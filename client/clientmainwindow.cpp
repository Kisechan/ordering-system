#include "clientmainwindow.h"
#include <QToolButton>
#include <QMessageBox>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QStyle>
#include <QTimer>
#include <QLayout>
#include <QPixmap>
#include <QDebug>
#include <QBoxLayout>
#include "Def.h"
#include "homepage.h"
#include "logindialog.h"
#include "cartpage.h"
#include "orderhistorypage.h"
#include "placeholderpage.h"
#include "ElaNavigationBar.h"
#include "ElaInteractiveCard.h"
#include "NetworkManager.h"
#include "accountpage.h"

ClientMainWindow::ClientMainWindow(NetworkManager* networkMgr, int tmpuserId, QString tmpusername, QWidget* parent)
    : ElaWindow(parent), m_networkMgr(networkMgr), userId(tmpuserId), username(tmpusername)
{
    QFont f = font();
    f.setPointSize(20);
    setFont(f);

    setWindowTitle(QStringLiteral("Online Shopping Mall(Client)"));
    resize(1050, 720);

    // 左侧导航栏（Ela 内置）
    setIsNavigationBarEnable(true);
    setNavigationBarDisplayMode(ElaNavigationType::Maximal);

    // 头像信息卡
    setUserInfoCardVisible(true);
    setUserInfoCardPixmap(QPixmap(QStringLiteral(":/include/Image/Moon.jpg")));
    setUserInfoCardTitle(username);
    setUserInfoCardSubTitle(QStringLiteral("自助点餐系统"));

    if (auto* nav = this->findChild<ElaNavigationBar*>()) {
        if (auto* card = nav->findChild<ElaInteractiveCard*>()) {
            card->setTitlePixelSize(20);
            card->update();
        }
    }

    // 点餐页面：可滚动菜品展示
    m_cart = new CartManager(this);
    m_home = new HomePage(m_networkMgr, this);
    addPageNode(QStringLiteral("点餐"), m_home, ElaIconType::House);

    connect(m_home, &HomePage::addToCartRequested,
            m_cart, [this](const Dish& dish, int qty) {
                m_cart->addDish(dish, qty);
            });


    // 购物车页面：展示已点菜品及结算
    auto* cartPage = new CartPage(m_cart, this);
    addPageNode(QStringLiteral("购物车"), cartPage, ElaIconType::CartShopping);

    connect(cartPage, &CartPage::orderRequested, this, [this](const OrderDraft& draft){
        // 点单逻辑，需要实现
        // 选了哪些菜品，每个菜品选了几个放在数据结构 m_cart（类型是 CartManager）里面了
        // ............................................................
    });

    // 历史订单页面：展示历史订单、评论、评分
    auto* orders = new OrderHistoryPage(QStringLiteral("00000001"), this);
    addPageNode(QStringLiteral("订单记录"), orders, ElaIconType::Receipt);

    connect(orders, &OrderHistoryPage::ordersRequested, this,
            [](const QString& userId){
                // 请求订单逻辑，需要请求得到 userId 的订单信息
                // ....................................................
            });

    connect(orders, &OrderHistoryPage::updateCommentRequested, this,
            [](const QString& userId, int orderId, const QString& c){
                // 更新评论逻辑，请求更新评论信息
                // ..................................................
            });


    // 底部 Footer：账号管理
    QString accountKey;
    auto* accountPage = new AccountPage(userId, username, this);

    addFooterNode(QStringLiteral("账号管理"),
                  accountPage,
                  accountKey,
                  0,
                  ElaIconType::UserGear);

    // ======== 三个功能：信号留给后端 ========
    connect(accountPage, &AccountPage::usernameUpdateRequested, this,
            [this, accountPage](int uid, const QString& newName){
                Q_UNUSED(uid);
                // ..........................................................
                // 后端修改用户名
                // 先本地演示：立刻改 UI
                username = newName;
                setUserInfoCardTitle(username);
                accountPage->setCurrentUsername(username);
            });

    connect(accountPage, &AccountPage::passwordUpdateRequested, this,
            [this](int uid, const QString& oldP, const QString& newP){
                Q_UNUSED(uid);
                Q_UNUSED(oldP);
                Q_UNUSED(newP);
                // 后端修改密码
                // .........................................................
                QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("密码修改请求已发出（待接后端）"));
            });

    connect(accountPage, &AccountPage::logoutRequested, this,
            [this](){
                // 1) 先隐藏当前主窗口（保证程序不退出）
                this->hide();

                // 2) 重新回到登录界面
                LoginDialog login(m_networkMgr);
                if (login.exec() == QDialog::Accepted) {
                    // 3) 登录成功：创建新主窗口
                    auto* w = new ClientMainWindow(m_networkMgr, login.getUserId(), login.getUsername(), nullptr);
                    w->setAttribute(Qt::WA_DeleteOnClose);
                    w->setGeometry(this->geometry()); // 可选：继承原窗口位置/大小
                    w->show();
                } else {
                    // 取消登录：直接退出程序
                    qApp->quit();
                }

                // 4) 关闭并销毁旧窗口
                this->setAttribute(Qt::WA_DeleteOnClose);
                this->close();
            });



    moveToCenter();
}

void ClientMainWindow::contextMenuEvent(QContextMenuEvent* e)
{
    QMenu menu(this);
    QAction* act = menu.addAction(QStringLiteral("刷新"));
    QAction* picked = menu.exec(e->globalPos());
    if (picked == act) hardRefresh();
}

void ClientMainWindow::hardRefresh()
{
    // 1) 先创建并 show 新窗口（避免关闭最后一个窗口导致退出）
    auto* w = new ClientMainWindow(m_networkMgr, userId, username, nullptr);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setGeometry(this->geometry());
    w->show();

    // 2) 再关闭当前窗口（建议让当前窗口也自动释放）
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->close();
}

