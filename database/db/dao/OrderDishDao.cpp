#include "OrderDishDao.h"
#include "DaoCommon.h"
#include "JsonUtil.h"
#include <QJsonArray>

namespace db {

QJsonObject OrderDishDao::insertOne(int orderId, int dishId, int count) {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    // count 字段必须赋值
    q.prepare("INSERT INTO t_order_dish(order_id, dish_id, count) VALUES(?, ?, ?)");
    q.addBindValue(orderId);
    q.addBindValue(dishId);
    q.addBindValue(count);

    if (!q.exec()) return makeErr(500, sqlErr(q));
    return makeOk(QJsonValue::Null);
}

QJsonObject OrderDishDao::deleteByOrderId(int orderId) {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("DELETE FROM t_order_dish WHERE order_id=?");
    q.addBindValue(orderId);

    if (!q.exec()) return makeErr(500, sqlErr(q));
    return makeOk(QJsonValue::Null);
}

QJsonObject OrderDishDao::listDishIdsByOrderId(int orderId) const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("SELECT dish_id FROM t_order_dish WHERE order_id=?");
    q.addBindValue(orderId);

    if (!q.exec()) return makeErr(500, sqlErr(q));

    QJsonArray arr;
    while (q.next()) arr.append(q.value(0).toInt());
    return makeOk(arr);
}

QJsonObject OrderDishDao::updateRating(int orderId, int dishId, int rating) {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("UPDATE t_order_dish SET rating=? WHERE order_id=? AND dish_id=?");
    q.addBindValue(rating);
    q.addBindValue(orderId);
    q.addBindValue(dishId);

    if (!q.exec()) return makeErr(500, sqlErr(q));
    return makeOk(QJsonValue::Null);
}


} // namespace db
