#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include "ElaWindow.h"

class OrderInfo_Page;
//class CallService_Page;
//class DishManage_Page;

class MainWindow : public ElaWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //void onNewOrderArrived();// 新订单弹窗
    //void onNewCallArrived();// 新呼叫弹窗

private:
    void initWindow();
    void initEdgeLayout();
    void initContent();

private:
    OrderInfo_Page *order_info_page_{nullptr};
    //CallService_Page *call_service_page_{nullptr};
    //DishManage_Page *dish_manage_page_{nullptr};

    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
