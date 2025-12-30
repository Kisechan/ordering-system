#pragma once
#ifndef SERVICEREQUEST_PAGE_H
#define SERVICEREQUEST_PAGE_H

#include "ElaScrollPage.h"
#include "servicerequestcard.h"
#include <QList>

class ElaLineEdit;
class ElaPushButton;
class QVBoxLayout;
class QWidget;
class QTimer;

class ServiceRequest_Page : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit ServiceRequest_Page(QWidget* parent = nullptr);

    void initializeTables();  // 初始化10个桌号
    int assignTable();        // 分配一个空闲桌号，返回桌号，-1表示无空闲桌号
    void setTableList(const QList<TableInfo>& tables);
    void addNewOrder(int tableNumber, int customerId, int orderId, const QList<DishInOrder>& dishes);
    void setTableCall(int tableNumber, bool hasCall);

signals:
    void refreshRequested();                   // 刷新
    void tableServed(int tableNumber);         // 桌号开始上菜
    void tableCompleted(int tableNumber);      // 桌号服务完成
    void callHandled(int tableNumber);         // 处理呼叫

private slots:
    void onRefresh();
    void onServeTable(int tableNumber);
    void onCompleteTable(int tableNumber);
    void onHandleCall(int tableNumber);
    void onDishServed(int tableNumber, const QString& dishName);

public slots:  // 公开onRefresh供mainwindow调用

private:
    void rebuildList(const QList<TableInfo>& tables);

    void setCardsEnabled(bool enabled);  // 启用/禁用所有卡片操作

private:
    QWidget*        m_listContainer = nullptr;
    QVBoxLayout*    m_listLayout = nullptr;
    ElaPushButton*  m_refreshBtn = nullptr;

    QList<TableInfo> m_allTables;  // 所有非空闲桌号
};

#endif // SERVICEREQUEST_PAGE_H
