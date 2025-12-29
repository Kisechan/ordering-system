#include "OrderQueryDao.h"
#include "DaoCommon.h"
#include "JsonUtil.h"
#include <QJsonArray>

namespace db {

QJsonObject OrderQueryDao::getOrderHistoryByUser(int userId) const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("SELECT order_id, user_id, total_amount, create_time, comment FROM t_order WHERE user_id=? ORDER BY create_time DESC");
    q.addBindValue(userId);

    if (!q.exec()) return makeErr(500, sqlErr(q));

    QJsonArray orders;
    while (q.next()) {
        const int orderId = q.value("order_id").toInt();
        QJsonObject o;
        o.insert("order_id", orderId);
        o.insert("user_id", q.value("user_id").toInt());
        o.insert("total_amount", q.value("total_amount").toDouble());
        o.insert("create_time", q.value("create_time").toDateTime().toString(Qt::ISODate));
        o.insert("comment", q.value("comment").toString());

        QSqlQuery qDish(db_);
        qDish.prepare(
            "SELECT d.dish_id, d.name, d.price, COUNT(*) AS cnt "
            "FROM t_order_dish od "
            "JOIN t_dish d ON od.dish_id=d.dish_id "
            "WHERE od.order_id=? "
            "GROUP BY d.dish_id, d.name, d.price "
            "ORDER BY d.dish_id ASC"
        );
        qDish.addBindValue(orderId);
        if (!qDish.exec()) return makeErr(500, sqlErr(qDish));

        QJsonArray dishes;
        while (qDish.next()) {
            QJsonObject item;
            item.insert("dish_id", qDish.value("dish_id").toInt());
            item.insert("name", qDish.value("name").toString());
            item.insert("price", qDish.value("price").toDouble());
            item.insert("count", qDish.value("cnt").toInt());
            dishes.append(item);
        }
        o.insert("dishes", dishes);
        orders.append(o);
    }

    return makeOk(orders);
}

QJsonObject OrderQueryDao::getAdminOrderBriefs() const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    if (!q.exec(
        "SELECT o.order_id, u.username, o.total_amount, o.create_time, o.comment "
        "FROM t_order o JOIN t_user u ON o.user_id=u.user_id "
        "ORDER BY o.order_id DESC"
    )) {
        return makeErr(500, sqlErr(q));
    }

    QJsonArray arr;
    while (q.next()) {
        QJsonObject b;
        b.insert("order_id", q.value("order_id").toInt());
        b.insert("username", q.value("username").toString());
        b.insert("total_amount", q.value("total_amount").toDouble());
        b.insert("create_time", q.value("create_time").toDateTime().toString(Qt::ISODate));
        b.insert("comment", q.value("comment").toString());
        arr.append(b);
    }
    return makeOk(arr);
}

} // namespace db
