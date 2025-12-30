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

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    ElaWindow(parent)
{
    initWindow();
    initEdgeLayout();

    // 监听页面切换事件，实现自动刷新
    connect(this, &ElaWindow::navigationNodeClicked,
            this, &MainWindow::onPageChanged);
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

        // TODO: 从数据库重新加载菜品列表
        qDebug() << "刷新菜品管理页面数据";
        // 示例：重新设置演示数据
        /*
        Dish d;
        d.dish_id = 1;
        d.name = QStringLiteral("宫保鸡丁");
        d.price = 28.00;
        d.category = QStringLiteral("川菜");
        d.rating = 4.8;
        d.url = QStringLiteral(":/Image/vvan.jpg");
        d.description = QStringLiteral("经典川菜，微辣香脆");
        dish_manage_page_->setDishList({d, d, d, d});
        */
    }

    // 当切换到订单管理页面时刷新数据
    if (nodeKey == QStringLiteral("订单管理") && order_info_page_) {

        // TODO: 从数据库重新加载订单列表
        qDebug() << "刷新订单管理页面数据";
        // 示例：设置演示订单数据
        /*
        Order o;
        o.order_id = 1001;
        o.user_id = 2001;
        o.total_amount = 88.00;
        o.create_time = QDateTime::fromString("2025-12-30 10:45:30", "yyyy-MM-dd hh:mm:ss");
        o.comment = QStringLiteral("少放辣椒，多加香菜");

        // 添加订单中的菜品
        OrderDish od1;
        od1.dish.dish_id = 1;
        od1.dish.name = QStringLiteral("宫保鸡丁");
        od1.dish.price = 28.00;
        od1.dish.url = QStringLiteral(":/Image/vvan.jpg");
        od1.quantity = 2;
        od1.customer_rating = 4.5;

        OrderDish od2;
        od2.dish.dish_id = 2;
        od2.dish.name = QStringLiteral("鱼香肉丝");
        od2.dish.price = 32.00;
        od2.dish.url = QStringLiteral(":/Image/vvan.jpg");
        od2.quantity = 1;
        od2.customer_rating = 5.0;

        o.dishes.append(od1);
        o.dishes.append(od2);

        order_info_page_->setOrderList({o, o, o});
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
