#include "dishcard.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QDebug>

#include "ElaImageCard.h"
#include "ElaText.h"
#include "ElaPushButton.h"

DishCard::DishCard(QWidget* parent)
    : QWidget(parent)
{
    QFont f;
    setObjectName("DishCardRoot");
    setStyleSheet(
        "#DishCardRoot { "
        "background-color:#f7f7f7;"
        "border-radius:12px;"
        "border:1px solid #d0d0d0;"
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

    auto* bottom = new QHBoxLayout();
    bottom->setContentsMargins(0, 6, 0, 0);
    bottom->setSpacing(10);

    m_minus = new ElaPushButton("-", this);
    m_minus->setFixedSize(34, 28);

    m_qtyText = new ElaText("1", 15, this);
    m_qtyText->setFixedWidth(24);
    m_qtyText->setAlignment(Qt::AlignCenter);

    m_plus = new ElaPushButton("+", this);
    m_plus->setFixedSize(34, 28);

    bottom->addWidget(m_minus);
    bottom->addWidget(m_qtyText);
    bottom->addWidget(m_plus);

    bottom->addStretch(1);

    m_price = new ElaText("", 24, this);
    m_price->setStyleSheet("color:#ff6a00;");
    bottom->addWidget(m_price);

    m_addBtn = new ElaPushButton(QStringLiteral("加入购物车"), this);
    f = m_addBtn->font();
    f.setPixelSize(16);
    m_addBtn->setFont(f);
    m_addBtn->setMinimumHeight(32);
    m_addBtn->setStyleSheet(
        "ElaPushButton { padding: 6px 14px; }"
    );
    bottom->addWidget(m_addBtn);

    right->addLayout(bottom);
    right->addStretch(1);

    root->addLayout(right, 1);

    connect(m_minus, &ElaPushButton::clicked, this, &DishCard::onMinus);
    connect(m_plus,  &ElaPushButton::clicked, this, &DishCard::onPlus);
    connect(m_addBtn,&ElaPushButton::clicked, this, &DishCard::onAddToCart);

    setMinimumHeight(150);
    refreshUI();
}

void DishCard::setDish(const Dish& d)
{
    m_dish = d;
    refreshUI();
}

void DishCard::setQuantity(int q)
{
    if (q < 1) q = 1;
    if (m_qty == q) return;
    m_qty = q;
    if (m_qtyText) m_qtyText->setText(QString::number(m_qty));
    emit quantityChanged(m_dish.dish_id, m_qty);
}

void DishCard::onMinus() { setQuantity(m_qty - 1); }
void DishCard::onPlus()  { setQuantity(m_qty + 1); }

void DishCard::onAddToCart()
{
    emit addToCartRequested(m_dish.dish_id, m_qty);
}

void DishCard::refreshUI()
{
    if (m_title) m_title->setText(m_dish.name.isEmpty() ? QStringLiteral("未命名菜品") : m_dish.name);

    if (m_meta) {
        m_meta->setText(QStringLiteral("品类：%1    评分：%2")
                        .arg(m_dish.category.isEmpty() ? QStringLiteral("-") : m_dish.category)
                        .arg(QString::number(m_dish.rating, 'f', 1)));
    }

    if (m_desc) m_desc->setText(m_dish.description);

    if (m_price) {
        m_price->setText(QStringLiteral("¥ %1").arg(QString::number(m_dish.price, 'f', 2)));
    }

    if (m_qtyText) m_qtyText->setText(QString::number(m_qty));

    // 图片：先支持资源路径(:/...)；后端接入后你再把网络图下载成 QImage setCardImage
    if (m_img) {
        QImage img;
        if (m_dish.url.startsWith(":/")) img = QImage(m_dish.url);
       // qDebug() << "img isNull=" << img.isNull() << " size=" << img.size() << " url=" << m_dish.url;
        if (!img.isNull()) m_img->setCardImage(img);
    }
}
