#include "clientmainwindow.h"

#include <QPixmap>

#include "Def.h"
#include "homepage.h"
#include "placeholderpage.h"

ClientMainWindow::ClientMainWindow(QWidget* parent)
    : ElaWindow(parent)
{
    QFont f = font();
    f.setPointSize(20);
    setFont(f);

    setWindowTitle(QStringLiteral("Online Shopping Mall(Client)"));
    resize(1200, 720);

    // 左侧导航栏（Ela 内置）
    setIsNavigationBarEnable(true);
    setNavigationBarDisplayMode(ElaNavigationType::Maximal);

    // 头像信息卡
    setUserInfoCardVisible(true);
    setUserInfoCardPixmap(QPixmap(QStringLiteral(":/include/Image/Moon.jpg"))); // 你换成自己的头像资源
    setUserInfoCardTitle(QStringLiteral("丰川祥子"));
    setUserInfoCardSubTitle(QStringLiteral("自助点餐系统"));

    // 右侧主页：先做一个“可滚动壳子”
    auto* home = new HomePage(this);
    addPageNode(QStringLiteral("主页"), home, ElaIconType::House);

    // 其它页面先占位
    addPageNode(QStringLiteral("购物车"),
                new PlaceholderPage(QStringLiteral("购物车（占位页）"), this),
                ElaIconType::CartShopping);

    addPageNode(QStringLiteral("收货地址"),
                new PlaceholderPage(QStringLiteral("收货地址（占位页）"), this),
                ElaIconType::MapLocationDot);

    addPageNode(QStringLiteral("订单记录"),
                new PlaceholderPage(QStringLiteral("订单记录（占位页）"), this),
                ElaIconType::Receipt);

    addPageNode(QStringLiteral("消息"),
                new PlaceholderPage(QStringLiteral("消息（占位页）"), this),
                ElaIconType::Message);

    // 底部 Footer：账号管理
    QString accountKey;
    addFooterNode(QStringLiteral("账号管理"),
                  new PlaceholderPage(QStringLiteral("账号管理（占位页）"), this),
                  accountKey,
                  0,
                  ElaIconType::UserGear);

    moveToCenter();
}
