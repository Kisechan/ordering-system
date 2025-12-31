#pragma once
#ifndef SERVICEREQUEST_PAGE_H
#define SERVICEREQUEST_PAGE_H

#include "ElaScrollPage.h"
#include "servicerequestcard.h"
#include <QList>
#include <QSqlDatabase>
#include <QMap>

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

    void setDatabase(const QSqlDatabase& db);
    void initializeTables();  // 初始化10个桌号
    void releaseTable(int tableNumber);  // 释放桌号
    void setTableList(const QList<TableInfo>& tables);
    void addNewOrder(int tableNumber, int customerId, int orderId, const QList<DishInOrder>& dishes);
    void setTableCall(int tableNumber, bool hasCall);

public slots:
    int assignTable(int userId);  // 给用户分配一个空闲桌号，返回桌号，-1表示无空闲桌号
    void onUserLogin(int userId);  // 用户登录时分配桌号
    void onSubmitOrder(int orderId, int userId);  // 用户提交订单时添加菜品
    void onCallWaiter(int userId);
    void onRefresh();

signals:
    void refreshRequested();                   // 刷新
    void tableServed(int tableNumber);         // 桌号开始上菜
    void tableCompleted(int tableNumber);      // 桌号服务完成
    void callHandled(int tableNumber);         // 处理呼叫

private slots:
    void onServeTable(int tableNumber);
    void onCompleteTable(int tableNumber);
    void onHandleCall(int tableNumber);
    void onDishServed(int tableNumber, const QString& dishName);

private:
    void rebuildList(const QList<TableInfo>& tables);
    void setCardsEnabled(bool enabled);  // 启用/禁用所有卡片操作
    int findTableByUserId(int userId);  // 根据用户ID查找桌号

private:
    QWidget*        m_listContainer = nullptr;
    QVBoxLayout*    m_listLayout = nullptr;
    ElaPushButton*  m_refreshBtn = nullptr;

    QList<TableInfo> m_allTables;  // 所有桌号（包括空闲和占用）
    QMap<int, int>   m_userToTable;  // 用户ID到桌号的映射
    QSqlDatabase     m_db;
};

#endif // SERVICEREQUEST_PAGE_H
