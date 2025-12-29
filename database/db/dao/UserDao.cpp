#include "UserDao.h"
#include "DaoCommon.h"
#include "JsonUtil.h"
#include <QDateTime>
#include <QJsonObject>

namespace db {

static QJsonObject rowToUser(const QSqlQuery& q) {
    QJsonObject u;
    u.insert("user_id", q.value("user_id").toInt());
    u.insert("username", q.value("username").toString());
    u.insert("password", q.value("password").toString());
    u.insert("register_time", q.value("register_time").toDateTime().toString(Qt::ISODate));
    return u;
}

QJsonObject UserDao::findByUsername(const QString& username) const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("SELECT user_id, username, password, register_time FROM t_user WHERE username=? LIMIT 1");
    q.addBindValue(username);

    if (!q.exec()) return makeErr(500, sqlErr(q));
    if (!q.next()) return makeOk(QJsonValue::Null);

    return makeOk(rowToUser(q));
}

QJsonObject UserDao::findById(int userId) const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("SELECT user_id, username, password, register_time FROM t_user WHERE user_id=? LIMIT 1");
    q.addBindValue(userId);

    if (!q.exec()) return makeErr(500, sqlErr(q));
    if (!q.next()) return makeOk(QJsonValue::Null);

    return makeOk(rowToUser(q));
}

QJsonObject UserDao::insertUser(const QString& username, const QString& password) const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("INSERT INTO t_user(username, password, register_time) VALUES(?, ?, ?)");
    q.addBindValue(username);
    q.addBindValue(password);
    q.addBindValue(QDateTime::currentDateTime());

    if (!q.exec()) return makeErr(500, sqlErr(q));
    return makeOk(QJsonValue::Null);
}

} // namespace db
