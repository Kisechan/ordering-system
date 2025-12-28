#pragma once
#include <QDateTime>
#include <QVector>
#include <QString>
#include "OrderDishItem.h"

namespace db {

class Order {
public:
    int orderId() const { return orderId_; }
    void setOrderId(int v) { orderId_ = v; }

    int userId() const { return userId_; }
    void setUserId(int v) { userId_ = v; }

    double totalAmount() const { return totalAmount_; }
    void setTotalAmount(double v) { totalAmount_ = v; }

    const QDateTime& createTime() const { return createTime_; }
    void setCreateTime(const QDateTime& v) { createTime_ = v; }

    const QString& comment() const { return comment_; }
    void setComment(const QString& v) { comment_ = v; }

    const QVector<OrderDishItem>& dishes() const { return dishes_; }
    void setDishes(const QVector<OrderDishItem>& v) { dishes_ = v; }
    void addDish(const OrderDishItem& item) { dishes_.push_back(item); }

private:
    int orderId_ = 0;
    int userId_ = 0;
    double totalAmount_ = 0.0;
    QDateTime createTime_;
    QString comment_;
    QVector<OrderDishItem> dishes_;
};

} // namespace db
