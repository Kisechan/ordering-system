#pragma once
#include <QSqlDatabase>
#include <QVector>
#include <QString>
#include "ApiResponse.h"
#include "../entity/DishCount.h"
#include "../entity/Order.h"
#include "../entity/AdminOrderBrief.h"

namespace db {

class OrderDao {
public:
    explicit OrderDao(QSqlDatabase db): db_(std::move(db)) {}

    // design.md 5.1 订单提交（需要事务 -> 非 const）
    ApiResponseVoid submitOrder(int userId, const QVector<DishCount>& dishes);

    // design.md 6.1 显示历史订单
    ApiResponse<QVector<Order>> listOrdersByUser(int userId) const;

    // design.md 6.2 提交评价
    ApiResponseVoid submitComment(int orderId, const QString& comment);

    // design.md 8.1 查询所有订单简要信息
    ApiResponse<QVector<AdminOrderBrief>> listAllOrdersBrief() const;

    // 删除订单（需要事务 -> 非 const）
    ApiResponseVoid deleteOrder(int orderId);

private:
    QSqlDatabase db_;
};

} // namespace db
