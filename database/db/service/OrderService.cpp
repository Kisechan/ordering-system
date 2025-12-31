#include "OrderService.h"
#include "../dao/DishDao.h"
#include "../dao/OrderDao.h"
#include "../dao/OrderDishDao.h"
#include "../dao/OrderQueryDao.h"
#include "../dao/JsonUtil.h"
#include <QSqlError>

namespace db {

QJsonObject OrderService::submitOrder(int userId, const QVector<DishCount>& dishes) {
    if (!db_.isValid() || !db_.isOpen())
        return makeErr(500, "Database is not open.");
    if (dishes.isEmpty())
        return makeErr(400, "dishes 不能为空");

    DishDao dishDao(db_);
    double total = 0.0;

    // 计算总金额
    for (const auto& it : dishes) {
        if (it.count() <= 0) continue;
        auto d = dishDao.findById(it.dishId());
        if (d.value("code").toInt() != 200) return d;
        if (d.value("data").isNull())
            return makeErr(404, QString("菜品不存在 dish_id=%1").arg(it.dishId()));
        total += d.value("data").toObject().value("price").toDouble() * it.count();
    }

    // 开启事务
    if (!db_.transaction())
        return makeErr(500, "开启事务失败: " + db_.lastError().text());

    OrderDao orderDao(db_);
    auto ins = orderDao.insertOrder(userId, total, "");
    if (ins.value("code").toInt() != 200) {
        db_.rollback();
        return ins;
    }

    const int orderId = ins.value("data").toObject().value("order_id").toInt();
    OrderDishDao odDao(db_);

    // 插入订单菜品
    for (const auto& it : dishes) {
        if (it.count() <= 0) continue;
        auto r = odDao.insertOne(orderId, it.dishId(), it.count()); // 传 count
        if (r.value("code").toInt() != 200) {
            db_.rollback();
            return r;
        }
    }

    // 提交事务
    if (!db_.commit()) {
        db_.rollback();
        return makeErr(500, "提交事务失败: " + db_.lastError().text());
    }

    QJsonObject data;
    data.insert("order_id", orderId);
    data.insert("total_amount", total);

    return makeOk(data, "订单成功提交");
}


QJsonObject OrderService::listOrdersByUser(int userId) const {
    OrderQueryDao dao(db_);
    auto r = dao.getOrderHistoryByUser(userId);
    if (r.value("code").toInt() == 200) r["msg"] = "get order history success";
    return r;
}

QJsonObject OrderService::submitComment(int orderId, const QString& comment, const QVector<int>& dishIdList, const QVector<int>& dishRatingList) {
    if (dishIdList.size() != dishRatingList.size()) {
        return makeErr(400, "菜品ID列表和评分列表长度不一致");
    }

    OrderDao orderDao(db_);
    OrderDishDao odDao(db_);

    // 1. 更新订单总体评价
    auto r = orderDao.updateComment(orderId, comment);
    if (r.value("code").toInt() != 200) return r;

    // 2. 更新每道菜的用户评分
    for (int i = 0; i < dishIdList.size(); ++i) {
        auto r2 = odDao.updateRating(orderId, dishIdList[i], dishRatingList[i]);
        if (r2.value("code").toInt() != 200) return r2;
    }

    return makeOk(QJsonValue::Null, "评价提交成功");
}

QJsonObject OrderService::listAdminOrdersBrief() const {
    OrderQueryDao dao(db_);
    auto r = dao.getAdminOrderBriefs();
    if (r.value("code").toInt() == 200) r["msg"] = "获取成功";
    return r;
}

} // namespace db
