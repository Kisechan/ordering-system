#pragma once
#include <QJsonObject>
#include "../dao/JsonUtil.h"

namespace db {

class ServiceService {
public:
    QJsonObject callWaiter(const QString& tableNo) const {
        return makeOk(QJsonValue::Null, QString("已呼叫服务员，桌号：%1").arg(tableNo));
    }
};

} // namespace db
