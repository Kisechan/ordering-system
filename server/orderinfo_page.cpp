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
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

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


}

OrderInfo_Page::~OrderInfo_Page() {
}

void OrderInfo_Page::onSearchTextChanged(const QString& text)
{
    m_keyword = text.trimmed();
    m_searchDebounce->start();
}

void OrderInfo_Page::setDatabase(const QSqlDatabase& db)
{
    m_db = db;
}

void OrderInfo_Page::loadOrdersFromDatabase()
{
    if (!m_db.isOpen()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            QStringLiteral("数据库连接未打开"), 2000, this);
        return;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT o.order_id, o.user_id, u.username, o.total_amount, o.create_time, o.comment
        FROM t_order o
        LEFT JOIN t_user u ON o.user_id = u.user_id
        ORDER BY o.create_time DESC
    )");

    if (!query.exec()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            QStringLiteral("查询订单失败: ") + query.lastError().text(), 2000, this);
        return;
    }

    QList<Order> orders;
    while (query.next()) {
        Order order;
        order.order_id = query.value("order_id").toInt();
        order.user_id = query.value("user_id").toInt();
        order.user_name = query.value("username").toString();
        order.total_amount = query.value("total_amount").toDouble();
        order.create_time = query.value("create_time").toDateTime();
        order.comment = query.value("comment").toString();

        orders.append(order);
    }

    setOrderList(orders);
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
    if (m_db.isOpen()) {
        loadOrdersFromDatabase();
        ElaMessageBar::success(ElaMessageBarType::BottomRight, QStringLiteral("提示"),
                              QStringLiteral("订单列表已刷新"), 1500, this);
    } else {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, QStringLiteral("提示"),
                              QStringLiteral("数据库连接未打开"), 1500, this);
    }
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

    dialog->setDatabase(m_db);

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
