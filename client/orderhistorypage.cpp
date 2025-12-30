#include "OrderHistoryPage.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include "ElaScrollArea.h"
#include "ElaText.h"
#include "ElaMessageBar.h"

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
    qDebug() << "[OrderHistoryPage] 评价提交成功";
    ElaMessageBar::success(ElaMessageBarType::BottomRight,
                          QStringLiteral("成功"),
                          QStringLiteral("评价提交成功"),
                          3000, this);
    
    // 重新加载订单列表以更新评论
    if (m_networkMgr) {
        qDebug() << "[OrderHistoryPage] 重新加载订单列表...";
        m_networkMgr->getOrderList();
    }
}

void OrderHistoryPage::onOrderCommentFailed(const QString& error)
{
    qDebug() << "[OrderHistoryPage] 评价提交失败:" << error;
    ElaMessageBar::error(ElaMessageBarType::BottomRight,
                        QStringLiteral("失败"),
                        QStringLiteral("评价提交失败: ") + error,
                        3000, this);
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

        connect(card, &OrderCard::editCommentRequested, this,
                [this, i](int orderId, const QString& current) {
                    qDebug() << "[OrderHistoryPage] 编辑订单评论 - orderId:" << orderId;
                    CommentDialog dlg(orderId, current, this);
                    if (dlg.exec() == QDialog::Accepted) {
                        const QString newC = dlg.comment();
                        qDebug() << "[OrderHistoryPage] 提交评论:" << newC;

                        if (m_networkMgr) {
                            m_networkMgr->submitOrderComment(orderId, newC);
                        } else {
                            ElaMessageBar::error(ElaMessageBarType::BottomRight,
                                               QStringLiteral("错误"),
                                               QStringLiteral("网络管理器未初始化"),
                                               2000, this);
                        }
                    }
                });

        connect(card, &OrderCard::rateRequested, this,
                [this, i](int orderId) {
                    qDebug() << "[OrderHistoryPage] 评分订单 - orderId:" << orderId;
                    RateDialog dlg(orderId,
                                   m_totalAmounts[i],
                                   m_times[i],
                                   m_orderItems[i],
                                   this);

                    if (dlg.exec() == QDialog::Accepted) {
                        // 获取 dish_id -> rating(1~5)
                        const QMap<int,int> ratings = dlg.ratings();
                        qDebug() << "[OrderHistoryPage] 获取评分数据，菜品数量:" << ratings.size();
                        
                        // 构造 dishes 数组
                        QJsonArray dishArray;
                        for (auto it = ratings.constBegin(); it != ratings.constEnd(); ++it) {
                            QJsonObject dishObj;
                            dishObj["dish_id"] = it.key();
                            dishObj["rating"] = it.value();
                            dishArray.append(dishObj);
                            qDebug() << "[OrderHistoryPage] 菜品评分 - dish_id:" << it.key() << ", rating:" << it.value();
                        }
                        
                        if (m_networkMgr) {
                            // 使用当前评论（如果有）+ 菜品评分
                            QString currentComment = m_comments[i];
                            m_networkMgr->submitOrderCommentWithRatings(orderId, currentComment, dishArray);
                        } else {
                            ElaMessageBar::error(ElaMessageBarType::BottomRight,
                                               QStringLiteral("错误"),
                                               QStringLiteral("网络管理器未初始化"),
                                               2000, this);
                        }
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


