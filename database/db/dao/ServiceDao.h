#pragma once
#include "ApiResponse.h"
#include <QString>

namespace db {

// design.md 4.2 呼叫服务员
// 数据库表里没有对应表，本 DAO 做成“业务接口占位”，方便你后续接前后端。
class ServiceDao {
public:
    ApiResponseVoid callWaiter(const QString& tableNo) const {
        ApiResponseVoid resp;
        resp.code = 200;
        resp.msg = QString("已呼叫服务员，桌号：%1").arg(tableNo);
        return resp;
    }
};

} // namespace db
