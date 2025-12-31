#pragma once
#ifndef ORDERDETAILDIALOG_H
#define ORDERDETAILDIALOG_H

#include "ElaContentDialog.h"
#include "ordercard.h"
#include <QSqlDatabase>

class QVBoxLayout;
class ElaText;
class ElaScrollArea;

class OrderDetailDialog : public ElaContentDialog
{
    Q_OBJECT
public:
    explicit OrderDetailDialog(QWidget* parent = nullptr);

    void setOrder(const Order& order);
    void setDatabase(const QSqlDatabase& db);

protected:
    void showEvent(QShowEvent* event) override;

private:
    void buildDishList();
    void loadOrderDetailFromDatabase();

private:
    Order m_order;
    QSqlDatabase m_db;
    QVBoxLayout* m_dishListLayout = nullptr;
    QWidget* m_dishListContainer = nullptr;

    // 订单信息标签
    ElaText* m_orderIdLabel = nullptr;
    ElaText* m_userLabel = nullptr;
    ElaText* m_timeLabel = nullptr;
    ElaText* m_commentLabel = nullptr;
    ElaText* m_totalLabel = nullptr;
};

#endif // ORDERDETAILDIALOG_H
