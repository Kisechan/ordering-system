#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "server.h"

#include "Def.h"
#include "ElaApplication.h"
#include "ElaMenu.h"
#include "ElaStatusBar.h"
#include "ElaText.h"

#include "orderinfo_page.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    ElaWindow(parent)
{
    initWindow();
    initEdgeLayout();
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

    qDebug()<<"order==ok";


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


void MainWindow::closeEvent(QCloseEvent *event) {
    Server* server = Server::getInstance();
    if (server && server->isListening()) {
        server->close();
        qDebug() << "Server closed";
    }
    event->accept();
}
