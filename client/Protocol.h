#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QString>
#include <QJsonObject>

namespace Protocol {

    // Message type constants - 用户认证
    constexpr const char* LOGIN = "login";
    constexpr const char* REGISTER = "register";

    // Message type constants - 菜品信息
    constexpr const char* DISH_LIST = "dish_list";

    // Message type constants - 订单相关
    constexpr const char* ORDER_SUBMIT = "order_submit";
    constexpr const char* ORDER_LIST = "order_list";
    constexpr const char* ORDER_COMMENT = "order_comment";
    constexpr const char* CALL_WAITER = "call_waiter";

    // Function declarations for building requests
    // 用户登录
    QJsonObject buildLoginRequest(const QString& username, const QString& password);
    // 用户注册
    QJsonObject buildRegisterRequest(const QString& username, const QString& password);
    // 获取菜品列表
    QJsonObject buildDishListRequest();
    // 呼叫服务员
    QJsonObject buildCallWaiterRequest();
    // 提交订单
    QJsonObject buildOrderSubmitRequest(const QJsonArray& dishes);
    // 获取订单历史
    QJsonObject buildOrderListRequest();
    // 提交订单评价
    QJsonObject buildOrderCommentRequest(int orderId, const QString& comment);

} // namespace Protocol

#endif // PROTOCOL_H
