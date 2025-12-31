#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSqlDatabase>

#include "ElaWindow.h"

class OrderInfo_Page;
class DishManage_Page;
class ServiceRequest_Page;

class MainWindow : public ElaWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setDatabase(const QSqlDatabase& db);

private slots:
    //void onNewOrderArrived();// 新订单弹窗
    //void onNewCallArrived();// 新呼叫弹窗
    void onPageChanged(ElaNavigationType::NavigationNodeType nodeType, QString nodeKey);

private:
    void initWindow();
    void initEdgeLayout();
    void initContent();

private:
    OrderInfo_Page *order_info_page_{nullptr};
    DishManage_Page *dish_manage_page_{nullptr};
    ServiceRequest_Page *service_request_page_{nullptr};

    QSqlDatabase m_db;

    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
