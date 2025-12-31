#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "server.h"

#include "Def.h"
#include "ElaApplication.h"
#include "ElaMenu.h"
#include "ElaStatusBar.h"
#include "ElaText.h"

#include "orderinfo_page.h"
#include "dishmanage_page.h"
#include "ordercard.h"
#include "servicerequest_page.h"
#include "servicerequestcard.h"
#include "servicerequest.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    ElaWindow(parent)
{
    initWindow();
    initEdgeLayout();

    // 监听页面切换事件，实现自动刷新
    connect(this, &ElaWindow::navigationNodeClicked,
            this, &MainWindow::onPageChanged);

    // 连接Server的callWaiter信号到ServiceRequest_Page
    Server* server = Server::getInstance();
    if (server && service_request_page_) {
        connect(server, &Server::callWaiter, service_request_page_, &ServiceRequest_Page::onCallWaiter);
        qDebug() << "已连接Server的callWaiter信号到ServiceRequest_Page";
    }

}

MainWindow::~MainWindow()
{

}

void MainWindow::initWindow() {
    // 开启云母效果
    eApp->setIsEnableMica(true);

    // 设置窗口标题
    setWindowTitle("Ordering-System(Server)");
    // 设置窗口左上方的图标
    setWindowIcon(QIcon(""));// 暂未设置图片
    // 设置窗口大小为1200*740
    resize(1024, 660);
    // 关闭用户资料显示
    setUserInfoCardVisible(false);
    // 关闭最大化按钮
    setWindowButtonFlags(getWindowButtonFlags() & ~ElaAppBarType::MaximizeButtonHint);
    // 关闭始终置顶按钮
    setWindowButtonFlags(getWindowButtonFlags() & ~ElaAppBarType::StayTopButtonHint);
    // 关闭回退按钮
    setWindowButtonFlags(getWindowButtonFlags() & ~ElaAppBarType::RouteBackButtonHint);


    setIsNavigationBarEnable(true);
    setNavigationBarDisplayMode(ElaNavigationType::Maximal);


    order_info_page_ = new OrderInfo_Page(this);
    addPageNode(QStringLiteral("订单管理"), order_info_page_, ElaIconType::Box);

    dish_manage_page_=new DishManage_Page(this);
    addPageNode(QStringLiteral("菜品管理"),dish_manage_page_,ElaIconType::House);

    service_request_page_ = new ServiceRequest_Page(this);
    addPageNode(QStringLiteral("服务请求"), service_request_page_, ElaIconType::Bell);



}

void MainWindow::initEdgeLayout() {
    //状态栏
    ElaStatusBar* statusBar = new ElaStatusBar(this);
    this->setStatusBar(statusBar);

    // 在状态栏显示uid
    ElaText* statusText = new ElaText("点餐管理系统  ", this);
    statusText->setTextPixelSize(20);
    statusBar->addWidget(statusText);

}

void MainWindow::initContent() {

}

void MainWindow::setDatabase(const QSqlDatabase& db)
{
    m_db = db;

    // 设置数据库到各个页面
    if (order_info_page_) {
        order_info_page_->setDatabase(m_db);
        // 初始加载订单数据
        order_info_page_->loadOrdersFromDatabase();
    }

    if (dish_manage_page_) {
        dish_manage_page_->setDatabase(m_db);
        // 初始加载菜品数据
        dish_manage_page_->loadDishesFromDatabase();
    }

    if (service_request_page_) {
        service_request_page_->setDatabase(m_db);
    }

}

/*
void MainWindow::onNewOrderArrived()
{
    order_info_page_->showNewOrderNotification();
}
*/

void MainWindow::onPageChanged(ElaNavigationType::NavigationNodeType /*nodeType*/, QString nodeKey)
{
    qDebug() << "页面切换到:" << nodeKey;

    // 当切换到菜品管理页面时刷新数据
    if (nodeKey == QStringLiteral("菜品管理") && dish_manage_page_) {
        qDebug() << "刷新菜品管理页面数据";
        // 从数据库重新加载菜品列表
        if (m_db.isOpen()) {
            dish_manage_page_->loadDishesFromDatabase();
        }
    }

    // 当切换到订单管理页面时刷新数据
    if (nodeKey == QStringLiteral("订单管理") && order_info_page_) {
        qDebug() << "刷新订单管理页面数据";
        // 从数据库重新加载订单列表
        if (m_db.isOpen()) {
            order_info_page_->loadOrdersFromDatabase();
        }
    }

    // 当切换到服务请求页面时刷新数据
    if (nodeKey == QStringLiteral("服务请求") && service_request_page_) {
        qDebug() << "刷新服务请求页面数据";

        /*
        service_request_page_->onRefresh();
        */

    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Server* server = Server::getInstance();
    if (server && server->isListening()) {
        server->close();
        qDebug() << "Server closed";
    }
    event->accept();
}
