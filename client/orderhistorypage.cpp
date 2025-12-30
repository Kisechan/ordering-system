#include "orderhistorypage.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include "ElaScrollArea.h"
#include "ElaText.h"
#include "ElaMessageBar.h"
#include "reviewdialog.h"
#include "ordercard.h"
#include "commentdialog.h"
#include "NetworkManager.h"

OrderHistoryPage::OrderHistoryPage(NetworkManager* networkMgr, QWidget* parent)
    : ElaScrollPage(parent), m_networkMgr(networkMgr)
{
    setTitleVisible(false);
    setPageTitleSpacing(0);

    m_content = new QWidget(this);
    auto* root = new QVBoxLayout(m_content);
    root->setContentsMargins(20, 18, 20, 18);
    root->setSpacing(12);

    // 列表容器
    m_listContainer = new QWidget(m_content);
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(12);
    root->addWidget(m_listContainer);

    root->addStretch(1);

    addCentralWidget(m_content, true, true);

    if (auto* sa = this->findChild<ElaScrollArea*>()) {
        sa->setAlignment(Qt::AlignTop);
    }

    // 初始化时清空数据
    m_orderIds.clear();
    m_totalAmounts.clear();
    m_times.clear();
    m_comments.clear();
    m_orderItems.clear();

    // 连接 NetworkManager 信号
    if (m_networkMgr) {
        connect(m_networkMgr, &NetworkManager::orderListReceived,
                this, &OrderHistoryPage::onOrderListReceived);
        connect(m_networkMgr, &NetworkManager::orderListError,
                this, &OrderHistoryPage::onOrderListError);
        connect(m_networkMgr, &NetworkManager::orderCommentSuccess,
                this, &OrderHistoryPage::onOrderCommentSuccess);
        connect(m_networkMgr, &NetworkManager::orderCommentFailed,
                this, &OrderHistoryPage::onOrderCommentFailed);
        
        qDebug() << "[OrderHistoryPage] 请求订单列表...";
        showEmpty(QStringLiteral("正在加载订单列表..."));
        m_networkMgr->getOrderList();
    } else {
        qDebug() << "[OrderHistoryPage] 错误：NetworkManager 为空";
        showEmpty(QStringLiteral("网络管理器未初始化"));
    }
}

void OrderHistoryPage::onOrderListReceived(const QJsonArray& orders)
{
    qDebug() << "[OrderHistoryPage] 收到订单列表，数量:" << orders.size();
    
    // 清空旧数据
    m_orderIds.clear();
    m_totalAmounts.clear();
    m_times.clear();
    m_comments.clear();
    m_orderItems.clear();

    for (const auto& v : orders) {
        if (!v.isObject()) continue;
        const QJsonObject o = v.toObject();

        const int orderId = o.value("order_id").toInt();
        const double totalAmount = o.value("total_amount").toDouble();
        const QString time = o.value("create_time").toString();
        const QString comment = o.value("comment").toString();

        qDebug() << "[OrderHistoryPage] 解析订单 -" 
                 << "orderId:" << orderId
                 << ", totalAmount:" << totalAmount
                 << ", time:" << time;

        QList<CartItem> items;
        const QJsonArray dishes = o.value("dishes").toArray();
        for (const auto& dv : dishes) {
            if (!dv.isObject()) continue;
            const QJsonObject di = dv.toObject();

            Dish d;
            d.dish_id = di.value("dish_id").toInt();
            d.name    = di.value("name").toString();
            d.price   = di.value("price").toDouble();
            d.category = di.value("category").toString();
            d.url     = di.value("url").toString();
            d.description = di.value("description").toString();
            
            CartItem it;
            it.dish = d;
            it.qty  = di.value("count").toInt(1);
            items.push_back(it);
        }

        m_orderIds.push_back(orderId);
        m_totalAmounts.push_back(totalAmount);
        m_times.push_back(time);
        m_comments.push_back(comment);
        m_orderItems.push_back(items);
    }

    rebuildList();
    
    if (m_orderIds.isEmpty()) {
        ElaMessageBar::information(ElaMessageBarType::BottomRight,
                                  QStringLiteral("提示"),
                                  QStringLiteral("暂无历史订单"),
                                  2000, this);
    } else {
        ElaMessageBar::success(ElaMessageBarType::BottomRight,
                              QStringLiteral("成功"),
                              QStringLiteral("订单列表加载成功"),
                              2000, this);
    }
}

void OrderHistoryPage::onOrderListError(const QString& error)
{
    qDebug() << "[OrderHistoryPage] 获取订单列表失败:" << error;
    showEmpty(QStringLiteral("获取订单列表失败: ") + error);
    ElaMessageBar::error(ElaMessageBarType::BottomRight,
                        QStringLiteral("错误"),
                        QStringLiteral("获取订单列表失败: ") + error,
                        3000, this);
}

void OrderHistoryPage::onOrderCommentSuccess()
{
    qDebug() << "[OrderHistoryPage] ========== 订单评价提交成功 ==========";
    ElaMessageBar::success(ElaMessageBarType::BottomRight,
                          QStringLiteral("成功"),
                          QStringLiteral("评价提交成功"),
                          3000, this);
    
    // 重新加载订单列表以更新评论
    if (m_networkMgr) {
        qDebug() << "[OrderHistoryPage] 重新加载订单列表...";
        m_networkMgr->getOrderList();
    }
    
    qDebug() << "[OrderHistoryPage] ===========================";
}

void OrderHistoryPage::onOrderCommentFailed(const QString& error)
{
    qDebug() << "[OrderHistoryPage] ========== 订单评价提交失败 ==========";
    qWarning() << "[OrderHistoryPage] 错误信息:" << error;
    ElaMessageBar::error(ElaMessageBarType::BottomRight,
                        QStringLiteral("失败"),
                        QStringLiteral("评价提交失败: ") + error,
                        3000, this);
    qDebug() << "[OrderHistoryPage] ===========================";
}

void OrderHistoryPage::rebuildList()
{
    clearList();

    if (m_orderIds.isEmpty()) {
        showEmpty(QStringLiteral("暂无历史订单"));
        return;
    }

    for (int i = 0; i < m_orderIds.size(); ++i) {
        auto* card = new OrderCard(m_listContainer);
        card->setOrder(m_orderIds[i], m_totalAmounts[i], m_times[i], m_comments[i], m_orderItems[i]);

        // 使用 orderId 而不是索引 i，避免 lambda 捕获的索引失效
        int orderId = m_orderIds[i];
        double totalAmount = m_totalAmounts[i];
        QString time = m_times[i];
        QString comment = m_comments[i];
        QList<CartItem> orderItems = m_orderItems[i];

        connect(card, &OrderCard::rateRequested, this,
                [this, i](int orderId) {
                    ReviewDialog dlg(orderId,
                                     m_totalAmounts[i],
                                     m_times[i],
                                     m_orderItems[i],
                                     m_comments[i],   // 预置评论：有就带进去，没有就是空
                                     this);

                    if (dlg.exec() == QDialog::Accepted) {
                        const QString newComment = dlg.comment();
                        const QMap<int,int> ratings = dlg.ratings(); // dish_id -> 1..5

                        // 先本地更新评论
                        m_comments[i] = newComment;
                        rebuildList();
                        // 后端实现 .....................................................

                    }
                });



        m_listLayout->addWidget(card);
    }
}

void OrderHistoryPage::clearList()
{
    if (!m_listLayout) return;
    while (m_listLayout->count() > 0) {
        QLayoutItem* it = m_listLayout->takeAt(0);
        if (!it) break;
        if (QWidget* w = it->widget()) {
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete it;
    }
}

void OrderHistoryPage::showEmpty(const QString& text)
{
    clearList();
    auto* t = new ElaText(text, 14, m_listContainer);
    t->setStyleSheet("color:#888888;");
    t->setAlignment(Qt::AlignHCenter);
    m_listLayout->addWidget(t);
}


