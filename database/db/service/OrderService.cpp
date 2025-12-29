#include "OrderService.h"
#include "../dao/DishDao.h"
#include "../dao/OrderDao.h"
#include "../dao/OrderDishDao.h"
#include "../dao/OrderQueryDao.h"
#include "../dao/JsonUtil.h"
#include <QSqlError>

namespace db {

QJsonObject OrderService::submitOrder(int userId, const QVector<DishCount>& dishes) {
    if (!db_.isValid() || !db_.isOpen()) return makeErr(500, "Database is not open.");
    if (dishes.isEmpty()) return makeErr(400, "dishes 不能为空");

    DishDao dishDao(db_);
    double total = 0.0;
    for (const auto& it : dishes) {
        if (it.count() <= 0) continue;
        auto d = dishDao.findById(it.dishId());
        if (d.value("code").toInt() != 200) return d;
        if (d.value("data").isNull()) return makeErr(404, QString("菜品不存在 dish_id=%1").arg(it.dishId()));
        total += d.value("data").toObject().value("price").toDouble() * it.count();
    }

    if (!db_.transaction()) return makeErr(500, "开启事务失败: " + db_.lastError().text());

    OrderDao orderDao(db_);
    auto ins = orderDao.insertOrder(userId, total, "");
    if (ins.value("code").toInt() != 200) { db_.rollback(); return ins; }
    const int orderId = ins.value("data").toObject().value("order_id").toInt();

    OrderDishDao odDao(db_);
    for (const auto& it : dishes) {
        for (int k = 0; k < it.count(); ++k) {
            auto r = odDao.insertOne(orderId, it.dishId());
            if (r.value("code").toInt() != 200) { db_.rollback(); return r; }
        }
    }

    if (!db_.commit()) { db_.rollback(); return makeErr(500, "提交事务失败: " + db_.lastError().text()); }
    return makeOk(QJsonValue::Null, "订单成功提交");
}

QJsonObject OrderService::listOrdersByUser(int userId) const {
    OrderQueryDao dao(db_);
    auto r = dao.getOrderHistoryByUser(userId);
    if (r.value("code").toInt() == 200) r["msg"] = "get order history success";
    return r;
}

QJsonObject OrderService::submitComment(int orderId, const QString& comment) {
    OrderDao dao(db_);
    auto r = dao.updateComment(orderId, comment);
    if (r.value("code").toInt() != 200) return r;
    return makeOk(QJsonValue::Null, "评价提交成功");
}

QJsonObject OrderService::listAdminOrdersBrief() const {
    OrderQueryDao dao(db_);
    auto r = dao.getAdminOrderBriefs();
    if (r.value("code").toInt() == 200) r["msg"] = "获取成功";
    return r;
}

} // namespace db
