#include "OrderHistoryPage.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

#include "ElaScrollArea.h"
#include "ElaText.h"

#include "OrderCard.h"
#include "CommentDialog.h"

OrderHistoryPage::OrderHistoryPage(const QString& userId, QWidget* parent)
    : ElaScrollPage(parent), m_userId(userId)
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

    // 构造即请求
    emit ordersRequested(m_userId);

    // ===== 先预置 2 个订单（调试用），确认 UI 没问题再接后端 =====
    // ===== 没问题了就删掉别忘了 =======
    m_orderIds.clear();
    m_totalAmounts.clear();
    m_times.clear();
    m_comments.clear();
    m_orderItems.clear();

    // ---- 订单 10001 ----
    {
        QList<CartItem> items;

        // 宫保鸡丁 x2
        Dish d1;
        d1.dish_id = 1;
        d1.name = QStringLiteral("宫保鸡丁");
        d1.price = 28.00;
        CartItem it1;
        it1.dish = d1;
        it1.qty = 2;
        items.push_back(it1);

        // 鱼香肉丝 x1
        Dish d2;
        d2.dish_id = 3;
        d2.name = QStringLiteral("鱼香肉丝");
        d2.price = 32.00;
        CartItem it2;
        it2.dish = d2;
        it2.qty = 1;
        items.push_back(it2);

        m_orderIds.push_back(10001);
        m_totalAmounts.push_back(88.00); // 28*2 + 32*1
        m_times.push_back(QStringLiteral("2025-01-03 12:45:30"));
        m_comments.push_back(QStringLiteral("味道很好，下次还来"));
        m_orderItems.push_back(items);
    }

    // ---- 订单 10002 ----
    {
        QList<CartItem> items;

        // 白切鸡 x1
        Dish d;
        d.dish_id = 2;
        d.name = QStringLiteral("白切鸡");
        d.price = 56.00;
        CartItem it;
        it.dish = d;
        it.qty = 1;
        items.push_back(it);

        m_orderIds.push_back(10002);
        m_totalAmounts.push_back(56.00);
        m_times.push_back(QStringLiteral("2025-01-05 18:20:10"));
        m_comments.push_back(QString()); // 没评论就不显示
        m_orderItems.push_back(items);
    }

    rebuildList();
}

void OrderHistoryPage::setOrdersJson(const QByteArray& json)
{
    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError || !doc.isArray()) {
        showEmpty(QStringLiteral("订单数据解析失败"));
        return;
    }

    // 清空旧数据
    m_orderIds.clear();
    m_totalAmounts.clear();
    m_times.clear();
    m_comments.clear();
    m_orderItems.clear();

    const QJsonArray arr = doc.array();
    for (const auto& v : arr) {
        if (!v.isObject()) continue;
        const QJsonObject o = v.toObject();

        const int orderId = o.value("order_id").toInt();
        const double totalAmount = o.value("total_amount").toDouble();
        const QString time = o.value("create_time").toString();
        const QString comment = o.value("comment").toString();

        QList<CartItem> items;
        const QJsonArray dishes = o.value("dishes").toArray();
        for (const auto& dv : dishes) {
            if (!dv.isObject()) continue;
            const QJsonObject di = dv.toObject();

            Dish d;
            d.dish_id = di.value("dish_id").toInt();
            d.name    = di.value("name").toString();
            d.price   = di.value("price").toDouble();
            // category/rating/url/description 后端若不给就留空（你不让新增结构，只能这样）
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
                    CommentDialog dlg(orderId, current, this);
                    if (dlg.exec() == QDialog::Accepted) {
                        const QString newC = dlg.comment();

                        // 先本地更新 UI（也可以等后端回调再改）
                        m_comments[i] = newC;
                        rebuildList();

                        emit updateCommentRequested(m_userId, orderId, newC);
                    }
                });

        connect(card, &OrderCard::rateRequested, this,
                [this](int orderId) {
                    // 评分：先留着
                    QMessageBox::information(this,
                                             QStringLiteral("提示"),
                                             QStringLiteral("评分功能稍后实现（order_id=%1）").arg(orderId));
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

void OrderHistoryPage::applyCommentUpdateResult(int orderId, bool ok, const QString& message)
{
    // 这里只做提示（你后端可以返回 ok/err）
    if (ok) {
        if (!message.isEmpty()) {
            QMessageBox::information(this, QStringLiteral("成功"), message);
        }
    } else {
        QMessageBox::warning(this, QStringLiteral("失败"),
                             message.isEmpty() ? QStringLiteral("评论更新失败") : message);
    }
}
