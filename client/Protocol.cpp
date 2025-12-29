#include "Protocol.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

namespace Protocol {

    // Helper function to get the current Unix timestamp
    static qint64 getCurrentTimestamp() {
        return QDateTime::currentSecsSinceEpoch();
    }

    // 用户登录请求
    QJsonObject buildLoginRequest(const QString& username, const QString& password) {
        QJsonObject request;
        request["type"] = LOGIN;
        QJsonObject data;
        data["username"] = username;
        data["password"] = password;
        request["data"] = data;
        request["timestamp"] = getCurrentTimestamp();
        return request;
    }

    // 用户注册请求
    QJsonObject buildRegisterRequest(const QString& username, const QString& password) {
        QJsonObject request;
        request["type"] = REGISTER;
        QJsonObject data;
        data["username"] = username;
        data["password"] = password;
        request["data"] = data;
        request["timestamp"] = getCurrentTimestamp();
        return request;
    }

    // 获取菜品列表请求
    QJsonObject buildDishListRequest() {
        QJsonObject request;
        request["type"] = DISH_LIST;
        request["data"] = QJsonObject(); // 空数据
        request["timestamp"] = getCurrentTimestamp();
        return request;
    }

    // 呼叫服务员请求
    QJsonObject buildCallWaiterRequest() {
        QJsonObject request;
        request["type"] = CALL_WAITER;
        request["data"] = QJsonObject(); // 空数据
        request["timestamp"] = getCurrentTimestamp();
        return request;
    }

    // 提交订单请求
    QJsonObject buildOrderSubmitRequest(const QJsonArray& dishes) {
        QJsonObject request;
        request["type"] = ORDER_SUBMIT;
        QJsonObject data;
        data["dishes"] = dishes;
        request["data"] = data;
        request["timestamp"] = getCurrentTimestamp();
        return request;
    }

    // 获取订单历史请求
    QJsonObject buildOrderListRequest() {
        QJsonObject request;
        request["type"] = ORDER_LIST;
        request["data"] = QJsonObject(); // 空数据
        request["timestamp"] = getCurrentTimestamp();
        return request;
    }

    // 提交订单评价请求
    QJsonObject buildOrderCommentRequest(int orderId, const QString& comment) {
        QJsonObject request;
        request["type"] = ORDER_COMMENT;
        QJsonObject data;
        data["order_id"] = orderId;
        data["comment"] = comment;
        request["data"] = data;
        request["timestamp"] = getCurrentTimestamp();
        return request;
    }

} // namespace Protocol