#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

namespace db {

inline QJsonObject makeResp(int code, const QString& msg, const QJsonValue& data = QJsonValue::Null) {
    QJsonObject o;
    o.insert("code", code);
    o.insert("msg", msg);
    o.insert("data", data);
    return o;
}

inline QJsonObject makeOk(const QJsonValue& data = QJsonValue::Null, const QString& msg = QString()) {
    return makeResp(200, msg, data);
}

inline QJsonObject makeErr(int code, const QString& msg) {
    return makeResp(code, msg, QJsonValue::Null);
}

} // namespace db
