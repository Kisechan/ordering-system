#include "OrderDao.h"
#include "DaoCommon.h"
#include <QDateTime>

namespace db {

ApiResponseVoid OrderDao::submitOrder(int userId, const QVector<DishCount>& dishes) {
    ApiResponseVoid resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }
    if (dishes.isEmpty()) { resp.code = 400; resp.msg = "dishes 不能为空"; return resp; }

    // total_amount 以数据库价格为准
    double total = 0.0;
    for (const auto& it : dishes) {
        if (it.count() <= 0) continue;

        QSqlQuery qPrice(db_);
        qPrice.prepare("SELECT price FROM t_dish WHERE dish_id=? LIMIT 1");
        qPrice.addBindValue(it.dishId());
        if (!qPrice.exec() || !qPrice.next()) {
            resp.code = 404;
            resp.msg = QString("菜品不存在 dish_id=%1").arg(it.dishId());
            return resp;
        }
        total += qPrice.value(0).toDouble() * it.count();
    }

    if (!db_.transaction()) {
        resp.code = 500;
        resp.msg = "开启事务失败: " + db_.lastError().text();
        return resp;
    }

    QSqlQuery qOrder(db_);
    qOrder.prepare("INSERT INTO t_order(user_id, total_amount, create_time, comment) VALUES(?, ?, ?, ?)");
    qOrder.addBindValue(userId);
    qOrder.addBindValue(total);
    qOrder.addBindValue(QDateTime::currentDateTime());
    qOrder.addBindValue(QString(""));

    if (!qOrder.exec()) {
        db_.rollback();
        resp.code = 500;
        resp.msg = "写入订单失败: " + sqlErr(qOrder);
        return resp;
    }

    const int orderId = qOrder.lastInsertId().toInt();

    // t_order_dish 无 count 字段：数量用多行表示
    QSqlQuery qOD(db_);
    qOD.prepare("INSERT INTO t_order_dish(order_id, dish_id) VALUES(?, ?)");

    for (const auto& it : dishes) {
        for (int k = 0; k < it.count(); ++k) {
            qOD.addBindValue(orderId);
            qOD.addBindValue(it.dishId());
            if (!qOD.exec()) {
                db_.rollback();
                resp.code = 500;
                resp.msg = "写入订单菜品失败: " + sqlErr(qOD);
                return resp;
            }
            qOD.finish();
        }
    }

    if (!db_.commit()) {
        db_.rollback();
        resp.code = 500;
        resp.msg = "提交事务失败: " + db_.lastError().text();
        return resp;
    }

    resp.code = 200;
    resp.msg = "订单成功提交";
    return resp;
}

ApiResponse<QVector<Order>> OrderDao::listOrdersByUser(int userId) const {
    ApiResponse<QVector<Order>> resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    QSqlQuery q(db_);
    q.prepare("SELECT order_id, user_id, total_amount, create_time, comment "
              "FROM t_order WHERE user_id=? ORDER BY create_time DESC");
    q.addBindValue(userId);

    if (!q.exec()) { resp.code = 500; resp.msg = sqlErr(q); return resp; }

    QVector<Order> orders;
    while (q.next()) {
        Order o;
        o.setOrderId(q.value("order_id").toInt());
        o.setUserId(q.value("user_id").toInt());
        o.setTotalAmount(q.value("total_amount").toDouble());
        o.setCreateTime(q.value("create_time").toDateTime());
        o.setComment(q.value("comment").toString());

        // Load dishes aggregated (count)
        QSqlQuery qDish(db_);
        qDish.prepare(
            "SELECT d.dish_id, d.name, d.price, COUNT(*) AS cnt "
            "FROM t_order_dish od "
            "JOIN t_dish d ON od.dish_id=d.dish_id "
            "WHERE od.order_id=? "
            "GROUP BY d.dish_id, d.name, d.price "
            "ORDER BY d.dish_id ASC"
        );
        qDish.addBindValue(o.orderId());
        if (!qDish.exec()) { resp.code = 500; resp.msg = sqlErr(qDish); return resp; }

        while (qDish.next()) {
            OrderDishItem item;
            item.setDishId(qDish.value("dish_id").toInt());
            item.setName(qDish.value("name").toString());
            item.setPrice(qDish.value("price").toDouble());
            item.setCount(qDish.value("cnt").toInt());
            o.addDish(item);
        }

        orders.push_back(o);
    }

    resp.code = 200;
    resp.msg = "get order history success";
    resp.data = orders;
    return resp;
}

ApiResponseVoid OrderDao::submitComment(int orderId, const QString& comment) {
    ApiResponseVoid resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    QSqlQuery q(db_);
    q.prepare("UPDATE t_order SET comment=? WHERE order_id=?");
    q.addBindValue(comment);
    q.addBindValue(orderId);

    if (!q.exec()) { resp.code = 500; resp.msg = sqlErr(q); return resp; }
    if (q.numRowsAffected() <= 0) { resp.code = 404; resp.msg = "订单不存在"; return resp; }

    resp.code = 200;
    resp.msg = "评价提交成功";
    return resp;
}

ApiResponse<QVector<AdminOrderBrief>> OrderDao::listAllOrdersBrief() const {
    ApiResponse<QVector<AdminOrderBrief>> resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    QSqlQuery q(db_);
    if (!q.exec(
        "SELECT o.order_id, u.username, o.total_amount, o.create_time, o.comment "
        "FROM t_order o JOIN t_user u ON o.user_id=u.user_id "
        "ORDER BY o.order_id DESC"
    )) {
        resp.code = 500;
        resp.msg = sqlErr(q);
        return resp;
    }

    QVector<AdminOrderBrief> out;
    while (q.next()) {
        AdminOrderBrief b;
        b.setOrderId(q.value("order_id").toInt());
        b.setUsername(q.value("username").toString());
        b.setTotalAmount(q.value("total_amount").toDouble());
        b.setCreateTime(q.value("create_time").toDateTime());
        b.setComment(q.value("comment").toString());
        out.push_back(b);
    }

    resp.code = 200;
    resp.msg = "获取成功";
    resp.data = out;
    return resp;
}

ApiResponseVoid OrderDao::deleteOrder(int orderId) {
    ApiResponseVoid resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    if (!db_.transaction()) {
        resp.code = 500;
        resp.msg = "开启事务失败: " + db_.lastError().text();
        return resp;
    }

    QSqlQuery q1(db_);
    q1.prepare("DELETE FROM t_order_dish WHERE order_id=?");
    q1.addBindValue(orderId);
    if (!q1.exec()) {
        db_.rollback();
        resp.code = 500;
        resp.msg = sqlErr(q1);
        return resp;
    }

    QSqlQuery q2(db_);
    q2.prepare("DELETE FROM t_order WHERE order_id=?");
    q2.addBindValue(orderId);
    if (!q2.exec()) {
        db_.rollback();
        resp.code = 500;
        resp.msg = sqlErr(q2);
        return resp;
    }

    if (!db_.commit()) {
        db_.rollback();
        resp.code = 500;
        resp.msg = "提交事务失败: " + db_.lastError().text();
        return resp;
    }

    resp.code = 200;
    resp.msg = "删除成功";
    return resp;
}

} // namespace db
