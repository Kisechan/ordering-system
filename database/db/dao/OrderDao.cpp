#include "OrderDao.h"
#include "DaoCommon.h"
#include "JsonUtil.h"
#include <QDateTime>
#include <QJsonArray>

namespace db {

static QJsonObject rowToOrderRow(const QSqlQuery& q) {
    QJsonObject o;
    o.insert("order_id", q.value("order_id").toInt());
    o.insert("user_id", q.value("user_id").toInt());
    o.insert("total_amount", q.value("total_amount").toDouble());
    o.insert("create_time", q.value("create_time").toDateTime().toString(Qt::ISODate));
    o.insert("comment", q.value("comment").toString());
    return o;
}

QJsonObject OrderDao::insertOrder(int userId, double totalAmount, const QString& comment) {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("INSERT INTO t_order(user_id, total_amount, create_time, comment) VALUES(?, ?, ?, ?)");
    q.addBindValue(userId);
    q.addBindValue(totalAmount);
    q.addBindValue(QDateTime::currentDateTime());
    q.addBindValue(comment);

    if (!q.exec()) return makeErr(500, sqlErr(q));

    QJsonObject data;
    data.insert("order_id", q.lastInsertId().toInt());
    return makeOk(data);
}

QJsonObject OrderDao::updateComment(int orderId, const QString& comment) {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("UPDATE t_order SET comment=? WHERE order_id=?");
    q.addBindValue(comment);
    q.addBindValue(orderId);

    if (!q.exec()) return makeErr(500, sqlErr(q));
    return makeOk(QJsonValue::Null);
}

QJsonObject OrderDao::listByUser(int userId) const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("SELECT order_id, user_id, total_amount, create_time, comment FROM t_order WHERE user_id=? ORDER BY create_time DESC");
    q.addBindValue(userId);

    if (!q.exec()) return makeErr(500, sqlErr(q));

    QJsonArray arr;
    while (q.next()) arr.append(rowToOrderRow(q));
    return makeOk(arr);
}

} // namespace db
