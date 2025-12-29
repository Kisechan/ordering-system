#include "CartDishCard.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>

#include "ElaImageCard.h"
#include "ElaText.h"

CartDishCard::CartDishCard(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("CartDishCardRoot");
    setStyleSheet(
        "#CartDishCardRoot {"
        "  background-color:#f7f7f7;"
        "  border-radius:12px;"
        "  border:1px solid #d0d0d0;"
        "}"
    );
    setAttribute(Qt::WA_StyledBackground, true);

    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(16, 14, 16, 14);
    root->setSpacing(16);

    // 左侧图片
    m_img = new ElaImageCard(this);
    m_img->setFixedSize(180, 120);
    m_img->setBorderRadius(10);
    m_img->setMaximumAspectRatio(1.0);
    m_img->setIsPreserveAspectCrop(true);
    root->addWidget(m_img, 0, Qt::AlignTop);

    // 右侧
    auto* right = new QVBoxLayout();
    right->setContentsMargins(0, 0, 0, 0);
    right->setSpacing(8);

    m_title = new ElaText("", 20, this);
    right->addWidget(m_title);

    m_meta = new ElaText("", 15, this);
    m_meta->setStyleSheet("color:#666666;");
    right->addWidget(m_meta);

    m_desc = new ElaText("", 15, this);
    m_desc->setStyleSheet("color:#666666;");
    right->addWidget(m_desc);

    // 底栏：数量 ×n + 小计价格
    auto* bottom = new QHBoxLayout();
    bottom->setContentsMargins(0, 6, 0, 0);
    bottom->setSpacing(12);

    m_qty = new ElaText("×1", 16, this);
    m_qty->setStyleSheet("color:#666666;");
    bottom->addWidget(m_qty);

    bottom->addStretch(1);

    m_price = new ElaText("", 24, this);
    m_price->setStyleSheet("color:#ff6a00;");
    bottom->addWidget(m_price);

    right->addLayout(bottom);
    right->addStretch(1);

    root->addLayout(right, 1);

    setMinimumHeight(150);
}

void CartDishCard::setItem(const CartItem& item)
{
    m_item = item;
    refreshUI();
}

void CartDishCard::refreshUI()
{
    const Dish& d = m_item.dish;

    if (m_title) m_title->setText(d.name.isEmpty() ? QStringLiteral("未命名菜品") : d.name);

    if (m_meta) {
        m_meta->setText(QStringLiteral("品类：%1    评分：%2")
                        .arg(d.category.isEmpty() ? QStringLiteral("-") : d.category)
                        .arg(QString::number(d.rating, 'f', 1)));
    }

    if (m_desc) m_desc->setText(d.description);

    if (m_qty) m_qty->setText(QStringLiteral("×%1").arg(m_item.qty));

    // 小计：单价 * 数量
    const double subtotal = d.price * double(m_item.qty);
    if (m_price) {
        m_price->setText(QStringLiteral("¥ %1").arg(QString::number(subtotal, 'f', 2)));
    }

    // 图
    if (m_img) {
        QImage img;
        if (d.url.startsWith(":/"))
            img = QImage(d.url);
        if (!img.isNull())
            m_img->setCardImage(img);
    }
}
