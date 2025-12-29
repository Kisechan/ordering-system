#pragma once
#ifndef ORDERCARD_H
#define ORDERCARD_H

#include <QString>
#include <QDateTime>
#include <QList>
#include "dishcard.h"

class ElaText;
class ElaPushButton;

// 订单菜品结构体（对应数据库 t_order_dish 表，假设新添加了quantity（该菜品点购数量）和customer_rating（用户对此次菜品的评分））
struct OrderDish {
    Dish    dish;               // 菜品信息
    int     quantity = 1;       // 该菜品点购数量
    double  customer_rating = 0.0; // 用户对此次菜品的评分 0.0为默认未评价
};

// 订单结构体（对应数据库 t_order 表）
struct Order {
    int         order_id = 0;
    int         user_id = 0;
    QString     user_name;          // 可以从用户表关联获取
    double      total_amount = 0.0;
    QDateTime   create_time;
    QString     comment;
    QList<OrderDish> dishes;            // 订单包含的菜品列表
};

class OrderCard : public QWidget
{
    Q_OBJECT
public:
    explicit OrderCard(QWidget* parent = nullptr);

    void setOrder(const Order& order);
    Order order() const { return m_order; }

signals:
    // 查看详情按钮
    void viewDetailRequested(int orderId);

private slots:

    void onViewDetail();

private:
    void refreshUI();

private:
    Order m_order;

    ElaText*        m_orderIdText = nullptr;
    ElaText*        m_userNameText = nullptr;
    ElaText*        m_totalAmountText = nullptr;
    ElaText*        m_createTimeText = nullptr;
    ElaText*        m_commentText = nullptr;
    ElaPushButton*  m_detailBtn = nullptr;

};

#endif // ORDERCARD_H
