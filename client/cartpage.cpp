#include "CartPage.h"

#include <QVBoxLayout>
#include <QWidget>

#include "ElaScrollArea.h"
#include "ElaText.h"
#include "ElaPushButton.h"

#include "CartDishCard.h"

CartPage::CartPage(CartManager* cart, QWidget* parent)
    : ElaScrollPage(parent), m_cart(cart)
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
    m_callBtn = new ElaPushButton(QStringLiteral("呼叫服务员（下单）"), content);

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

    // 下单：把购物车打包成 OrderDraft 发出去（你后端接这个信号）
    connect(m_callBtn, &ElaPushButton::clicked, this, [this]() {
        if (!m_cart) return;
        const auto draft = m_cart->buildOrderDraft();
        emit orderRequested(draft);
        // 你想下单成功后清空，就在后端回调成功时 m_cart->clear();
    });

    reloadFromCart();
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

