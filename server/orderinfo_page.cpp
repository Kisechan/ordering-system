#include "orderinfo_page.h"
#include "ordercard.h"
#include "orderdetaildialog.h"

#include "ElaMessageBar.h"
#include "ElaText.h"
#include "ElaPushButton.h"
#include "ElaLineEdit.h"
#include "ElaText.h"
#include "ElaScrollArea.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTimer>

OrderInfo_Page::OrderInfo_Page(QWidget* parent):
    ElaScrollPage(parent)
{
    setTitleVisible(false);
    setPageTitleSpacing(0);

    // ===== 滚动内容根容器 =====
    auto* content = new QWidget(this);
    auto* root = new QVBoxLayout(content);
    root->setContentsMargins(20, 18, 20, 18);
    root->setSpacing(12);

    // ===== 顶部操作栏 =====
    auto* topBar = new QWidget(content);
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(10);

    auto* title = new ElaText(QStringLiteral("订单管理"), 22, topBar);
    title->setStyleSheet("font-weight: bold;");

    m_searchEdit = new ElaLineEdit(topBar);
    m_searchEdit->setPlaceholderText(QStringLiteral("搜索订单号 / 用户名 / 评论"));
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumHeight(36);
    m_searchEdit->setFixedWidth(260);

    m_refreshBtn = new ElaPushButton(QStringLiteral("刷新"), topBar);
    m_refreshBtn->setFixedSize(80, 36);

    topLayout->addWidget(title);
    topLayout->addStretch();
    topLayout->addWidget(m_searchEdit);
    topLayout->addWidget(m_refreshBtn);

    root->addWidget(topBar);

    // ===== 订单列表容器（一行一个订单卡片）=====
    m_listContainer = new QWidget(content);
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(12);
    root->addWidget(m_listContainer);

    // 让内容从顶部开始排（空白永远在下面）
    root->addStretch(1);

    // 把 content 挂到 ElaScrollPage 内部滚动体系
    addCentralWidget(content, /*isWidgetResizeable=*/true, /*isVerticalGrabGesture=*/true);

    // 内容少时也贴顶（防止被居中）
    if (auto* sa = this->findChild<ElaScrollArea*>()) {
        sa->setAlignment(Qt::AlignTop);
    }

    // ===== 搜索：防抖过滤 + 回车给后端 =====
    m_searchDebounce = new QTimer(this);
    m_searchDebounce->setSingleShot(true);
    m_searchDebounce->setInterval(200);

    connect(m_searchEdit, &ElaLineEdit::textChanged, this, &OrderInfo_Page::onSearchTextChanged);
    connect(m_searchDebounce, &QTimer::timeout, this, &OrderInfo_Page::applyFilterNow);
    connect(m_searchEdit, &ElaLineEdit::returnPressed, this, &OrderInfo_Page::onSearchReturnPressed);
    connect(m_refreshBtn, &ElaPushButton::clicked, this, &OrderInfo_Page::onRefresh);

    // 注意：！！！订单总金额和具体菜品小计总和没有校准（总金额是直接传过来的，各菜品小计是单价*数量计算）
    // ===== 演示：添加示例订单数据 =====
    QList<Order> demoOrders;

    Order o1;
    o1.order_id = 1;
    o1.user_id = 2;
    o1.user_name = QStringLiteral("张三");
    o1.total_amount = 88.00;
    o1.create_time = QDateTime::fromString("2025-01-03 12:45:30", "yyyy-MM-dd hh:mm:ss");
    o1.comment = QStringLiteral("味道很好，下次还来");

    // 添加订单菜品（演示用
    Dish d1;
    d1.dish_id = 1;
    d1.name = QStringLiteral("宫保鸡丁");
    d1.price = 28.00;
    d1.category = QStringLiteral("川菜");
    d1.rating = 4.8;
    d1.url = QStringLiteral(":/Image/vvan.jpg");
    d1.description = QStringLiteral("经典川菜，微辣香脆");

    OrderDish od1;
    od1.dish = d1;
    od1.quantity = 2;           // 点了2份
    od1.customer_rating = 5.0;  // 客户给5分

    OrderDish od2;
    od2.dish = d1;
    od2.quantity = 1;
    od2.customer_rating = 4.5;

    o1.dishes.append(od1);

    Order o2;
    o2.order_id = 2;
    o2.user_id = 3;
    o2.user_name = QStringLiteral("李四");
    o2.total_amount = 56.00;
    o2.create_time = QDateTime::fromString("2025-01-05 18:20:10", "yyyy-MM-dd hh:mm:ss");
    o2.comment = "";
    o2.dishes.append(od2);

    Order o3;
    o3.order_id = 3;
    o3.user_id = 2;
    o3.user_name = QStringLiteral("张三");
    o3.total_amount = 64.00;
    o3.create_time = QDateTime::fromString("2025-01-06 19:05:00", "yyyy-MM-dd hh:mm:ss");
    o3.comment = QStringLiteral("所以325是什么意思？？🤔难道是指明日方舟up主魔法ZC目录2024年3月11日的直播，彼时由龙哥哥今天又鸽了主办的明日方舟集成战略民间赛事仙术杯第五届正在如火如荼地展开中，本届仙术杯采取团队赛的形式，ZC 代表冠军厨小队的第二位出战，使用科学分队焰影苇草开但由于临场过于紧张运气Maybe和肉鸽基础不够扎实等原因在第三层关底利刃所指暴毙，局内结算325分啊，我还以为是出自明日方舟up主魔法ZC目录2024年3月11日的直播，彼时由龙哥哥今天又鸽了主办的明日方舟集成战略民间赛事仙术杯第五届正在如火如荼地展开中，本届仙术杯采取团队赛的形式，ZC 代表冠军厨小队的第二位出战，使用科学分队焰影苇草开但由于临场过于紧张运气Maybe和肉鸽基础不够扎实等原因在第三层关底利刃所指暴毙，局内结算325分吗？🤔");

    OrderDish od3;
    od3.dish = d1;
    od3.quantity = 2;
    od3.customer_rating = 0.0;  // 默认0.0为未评分
    o3.dishes.append(od3);

    demoOrders << o1 << o2 << o3 ;

    setOrderList(demoOrders);
}

OrderInfo_Page::~OrderInfo_Page() {
}

void OrderInfo_Page::onSearchTextChanged(const QString& text)
{
    m_keyword = text.trimmed();
    m_searchDebounce->start();
}

void OrderInfo_Page::setOrderList(const QList<Order>& orders)
{
    m_allOrders = orders;
    applyFilterNow();
}

void OrderInfo_Page::onSearchReturnPressed()
{
    emit searchRequested(m_keyword); // 后端接口预留：回车再请求服务器
}

void OrderInfo_Page::onRefresh()
{
    emit refreshOrderRequested();
    ElaMessageBar::success(ElaMessageBarType::BottomRight, QStringLiteral("提示"),
                          QStringLiteral("正在刷新订单列表..."), 1500, this);
}

void OrderInfo_Page::applyFilterNow()
{
    QList<Order> show;
    if (m_keyword.isEmpty()) {
        show = m_allOrders;
    } else {
        for (const auto& order : m_allOrders) {
            // 搜索订单号、用户名、备注
            QString orderIdStr = QString::number(order.order_id);
            if (orderIdStr.contains(m_keyword, Qt::CaseInsensitive) ||
                order.user_name.contains(m_keyword, Qt::CaseInsensitive) ||
                order.comment.contains(m_keyword, Qt::CaseInsensitive)) {
                show.push_back(order);
            }
        }
    }
    rebuildList(show);
}

void OrderInfo_Page::onViewOrderDetail(int orderId)
{
    // 从订单列表中查找对应订单
    Order* foundOrder = nullptr;
    for (auto& order : m_allOrders) {
        if (order.order_id == orderId) {
            foundOrder = &order;
            break;
        }
    }

    if (!foundOrder) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, QStringLiteral("提示"),
                              QStringLiteral("订单未找到"), 2000, this);
        return;
    }

    // 显示订单详情对话框
    auto* dialog = new OrderDetailDialog(this);
    dialog->setOrder(*foundOrder);
    dialog->exec();
    dialog->deleteLater();
}

void OrderInfo_Page::rebuildList(const QList<Order>& orders)
{
    if (!m_listLayout) return;

    // 清空旧卡片
    while (m_listLayout->count() > 0) {
        QLayoutItem* it = m_listLayout->takeAt(0);
        if (!it) break;
        if (QWidget* w = it->widget()) {
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete it;
    }

    if (orders.isEmpty()) {
        // 空态
        auto* empty = new ElaText(QStringLiteral("没有找到相关订单"), 14, m_listContainer);
        empty->setStyleSheet("color:#888888;");
        empty->setAlignment(Qt::AlignHCenter);
        m_listLayout->addWidget(empty);
        return;
    }

    for (const auto& order : orders) {
        auto* card = new OrderCard(m_listContainer);
        card->setOrder(order);

        connect(card, &OrderCard::viewDetailRequested, this, &OrderInfo_Page::onViewOrderDetail);

        m_listLayout->addWidget(card);
    }
}
