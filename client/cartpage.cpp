#include "cartpage.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

#include "ElaScrollArea.h"
#include "ElaText.h"
#include "ElaPushButton.h"
#include "ElaMessageBar.h"

#include "cartdishcard.h"
#include "NetworkManager.h"

CartPage::CartPage(CartManager* cart, NetworkManager* networkMgr, QWidget* parent)
    : ElaScrollPage(parent), m_cart(cart), m_networkMgr(networkMgr)
{
    setTitleVisible(false);
    setPageTitleSpacing(0);

    // ===== 一个 scrollball：内容可滚动 =====
    auto* content = new QWidget(this);
    auto* root = new QVBoxLayout(content);
    root->setContentsMargins(20, 18, 20, 18);
    root->setSpacing(12);

    // 卡片列表容器
    m_listContainer = new QWidget(content);
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(12);
    root->addWidget(m_listContainer);

    // 呼叫服务员按钮
    m_callBtn = new ElaPushButton(QStringLiteral("下单"), content);

    QFont f = m_callBtn->font();
    f.setPixelSize(19);
    m_callBtn->setFont(f);

    m_callBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_callBtn->setStyleSheet("ElaPushButton { padding: 8px 18px; }");
    m_callBtn->setFixedHeight(45);

    auto* btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 8, 0, 0);
    btnRow->addStretch(1);
    btnRow->addWidget(m_callBtn);
    btnRow->addStretch(1);

    root->addLayout(btnRow);


    // 空白永远在下面（保证贴顶）
    root->addStretch(1);

    addCentralWidget(content, true, true);

    if (auto* sa = this->findChild<ElaScrollArea*>()) {
        sa->setAlignment(Qt::AlignTop);
    }

    // 购物车变化 -> 刷新
    if (m_cart) {
        connect(m_cart, &CartManager::cartChanged, this, &CartPage::reloadFromCart);
    }

    // 下单按钮点击
    connect(m_callBtn, &ElaPushButton::clicked, this, &CartPage::onSubmitOrder);
    
    // 连接 NetworkManager 信号
    if (m_networkMgr) {
        connect(m_networkMgr, &NetworkManager::orderSubmitSuccess,
                this, &CartPage::onOrderSuccess);
        connect(m_networkMgr, &NetworkManager::orderSubmitFailed,
                this, &CartPage::onOrderFailed);
    }

    reloadFromCart();
}

void CartPage::onSubmitOrder()
{
    qDebug() << "[CartPage] 点击下单按钮";
    
    if (!m_cart || m_cart->isEmpty()) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, 
                              QStringLiteral("提示"),
                              QStringLiteral("购物车为空，请先添加菜品"), 
                              2000, this);
        qDebug() << "[CartPage] 购物车为空，无法下单";
        return;
    }
    
    if (!m_networkMgr) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, 
                            QStringLiteral("错误"),
                            QStringLiteral("网络管理器未初始化"), 
                            2000, this);
        qDebug() << "[CartPage] 网络管理器未初始化";
        return;
    }
    
    if (!m_networkMgr->isConnected()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, 
                            QStringLiteral("错误"),
                            QStringLiteral("未连接到服务器"), 
                            2000, this);
        qDebug() << "[CartPage] 未连接到服务器";
        return;
    }
    
    // 构造订单数据：QJsonArray，每个元素为 {"dish_id": int, "count": int}
    QJsonArray dishArray;
    const auto items = m_cart->items();
    
    for (const auto& item : items) {
        QJsonObject dishObj;
        dishObj["dish_id"] = item.dish.dish_id;
        dishObj["count"] = item.qty;
        dishArray.append(dishObj);
        
        qDebug() << "[CartPage] 添加到订单: dish_id=" << item.dish.dish_id 
                 << ", count=" << item.qty 
                 << ", name=" << item.dish.name 
                 << ", price=" << item.dish.price;
    }
    
    // 记录订单摘要
    qDebug() << "[CartPage] 提交订单: 共" << items.size() << "种菜品, 总数量" 
             << m_cart->totalCount() << ", 总金额¥" << m_cart->totalPrice();
    
    // 禁用按钮，显示提交中状态
    m_callBtn->setEnabled(false);
    ElaMessageBar::information(ElaMessageBarType::BottomRight, 
                              QStringLiteral("提示"),
                              QStringLiteral("订单提交中..."), 
                              1500, this);
    
    // 调用 NetworkManager 提交订单
    m_networkMgr->submitOrder(dishArray);
}

void CartPage::onOrderSuccess()
{
    qDebug() << "[CartPage] 订单提交成功！";
    
    m_callBtn->setEnabled(true);
    ElaMessageBar::success(ElaMessageBarType::BottomRight, 
                          QStringLiteral("成功"),
                          QStringLiteral("订单提交成功！"), 
                          3000, this);
    
    // 清空购物车
    if (m_cart) {
        m_cart->clear();
        qDebug() << "[CartPage] 购物车已清空";
    }
    
    // 发出信号给其他模块（可选）
    const auto draft = m_cart ? m_cart->buildOrderDraft() : OrderDraft();
    emit orderRequested(draft);
}

void CartPage::onOrderFailed(const QString& error)
{
    qDebug() << "[CartPage] 订单提交失败: " << error;
    
    m_callBtn->setEnabled(true);
    ElaMessageBar::error(ElaMessageBarType::BottomRight, 
                        QStringLiteral("失败"),
                        QStringLiteral("订单提交失败: ") + error, 
                        3000, this);
}

void CartPage::showMessage(const QString& msg, int duration)
{
    // 清空现有状态文本
    if (m_statusText) {
        m_listLayout->removeWidget(m_statusText);
        m_statusText->deleteLater();
        m_statusText = nullptr;
    }
    
    // 显示新消息
    m_statusText = new ElaText(msg, 13, m_listContainer);
    m_statusText->setStyleSheet("color:#0078d4;");
    m_statusText->setAlignment(Qt::AlignHCenter);
    
    // 在列表顶部插入消息
    if (m_listLayout) {
        m_listLayout->insertWidget(0, m_statusText);
    }
}

void CartPage::reloadFromCart()
{
    if (!m_cart) {
        rebuildList({});
        return;
    }
    rebuildList(m_cart->items());
}

void CartPage::rebuildList(const QList<CartItem>& items)
{
    if (!m_listLayout) return;

    // 清空旧卡片
    while (m_listLayout->count() > 0) {
        QLayoutItem* it = m_listLayout->takeAt(0);
        if (!it) break;
        if (QWidget* w = it->widget()) {
            // 如果要删除的是状态文本，重置指针
            if (w == m_statusText) {
                m_statusText = nullptr;
            }
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete it;
    }

    if (items.isEmpty()) {
        auto* empty = new ElaText(QStringLiteral("购物车为空"), 14, m_listContainer);
        empty->setStyleSheet("color:#888888;");
        empty->setAlignment(Qt::AlignHCenter);
        m_listLayout->addWidget(empty);
        return;
    }

    for (const auto& ci : items) {
        auto* card = new CartDishCard(m_listContainer);
        card->setItem(ci);
        m_listLayout->addWidget(card);
    }
}

