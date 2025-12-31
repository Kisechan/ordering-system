#pragma once
#ifndef SERVICEREQUEST_H
#define SERVICEREQUEST_H

#include <QString>
#include <QList>
#include "dishcard.h"

// 桌号服务状态枚举
enum class TableServiceStatus {
    Pending,      // 待处理
    Processing,   // 正在处理
    Completed     // 已完成
};

// 订单中的菜品（用于上菜）
struct DishInOrder {
    Dish dish;           // 菜品信息
    int quantity;        // 数量
    bool isServed;       // 是否已送达

    DishInOrder() : quantity(1), isServed(false) {}
};

// 桌号信息结构
struct TableInfo {
    int table_number;                    // 桌号
    int customer_id;                     // 顾客ID（0表示空闲）
    int order_id;                        // 订单号
    TableServiceStatus status;           // 服务状态
    bool is_idle;                        // 是否空闲（空闲则不在UI显示）
    bool has_call;                       // 是否有呼叫
    QList<DishInOrder> dishes;          // 待上菜品列表

    TableInfo()
        : table_number(0)
        , customer_id(0)
        , order_id(0)
        , status(TableServiceStatus::Pending)
        , is_idle(true)
        , has_call(false)
    {}
};



#endif // SERVICEREQUEST_H
