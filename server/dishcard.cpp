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

    // 中间信息区域
    auto* middle = new QVBoxLayout();
    middle->setContentsMargins(0, 0, 0, 0);
    middle->setSpacing(8);

    m_title = new ElaText("", 20, this);
    m_title->setStyleSheet("font-weight: bold;");
    middle->addWidget(m_title);

    m_dishId = new ElaText("", 13, this);
    m_dishId->setStyleSheet("color:#999999;");
    middle->addWidget(m_dishId);

    m_category = new ElaText("", 15, this);
    m_category->setStyleSheet("color:#666666;");
    middle->addWidget(m_category);

    m_rating = new ElaText("", 15, this);
    m_rating->setStyleSheet("color:#ff9500; font-weight: bold;");
    middle->addWidget(m_rating);

    m_desc = new ElaText("", 14, this);
    m_desc->setStyleSheet("color:#888888;");
    m_desc->setWordWrap(true);
    middle->addWidget(m_desc);

    middle->addStretch(1);

    root->addLayout(middle, 1);

    // 右侧操作区域
    auto* right = new QVBoxLayout();
    right->setContentsMargins(0, 0, 0, 0);
    right->setSpacing(12);

    m_price = new ElaText("", 26, this);
    m_price->setStyleSheet("color:#ff6a00; font-weight: bold;");
    right->addWidget(m_price);

    right->addStretch(1);

    // 按钮区域
    m_editBtn = new ElaPushButton(QStringLiteral("修改"), this);
    f = m_editBtn->font();
    f.setPixelSize(15);
    m_editBtn->setFont(f);
    m_editBtn->setFixedSize(90, 34);
    m_editBtn->setStyleSheet(
        "ElaPushButton { padding: 6px 12px; }"
    );

    m_deleteBtn = new ElaPushButton(QStringLiteral("删除"), this);
    m_deleteBtn->setFont(f);
    m_deleteBtn->setFixedSize(90, 34);
    m_deleteBtn->setStyleSheet(
        "ElaPushButton { "
        "padding: 6px 12px; "
        "background-color: #ff4444;"
        "}"
    );

    right->addWidget(m_editBtn);
    right->addWidget(m_deleteBtn);

    root->addLayout(right);

    connect(m_editBtn, &ElaPushButton::clicked, this, &DishCard::onEdit);
    connect(m_deleteBtn, &ElaPushButton::clicked, this, &DishCard::onDelete);

    setMinimumHeight(150);
    refreshUI();
}

void DishCard::setDish(const Dish& d)
{
    m_dish = d;
    refreshUI();
}

void DishCard::onEdit()
{
    emit editRequested(m_dish.dish_id);
}

void DishCard::onDelete()
{
    emit deleteRequested(m_dish.dish_id);
}

void DishCard::refreshUI()
{
    if (m_title) {
        m_title->setText(m_dish.name.isEmpty() ? QStringLiteral("未命名菜品") : m_dish.name);
    }

    if (m_dishId) {
        m_dishId->setText(QStringLiteral("ID: %1").arg(m_dish.dish_id));
    }

    if (m_category) {
        m_category->setText(QStringLiteral("品类：%1")
                           .arg(m_dish.category.isEmpty() ? QStringLiteral("-") : m_dish.category));
    }

    if (m_rating) {
        m_rating->setText(QStringLiteral("评价分数：%1").arg(QString::number(m_dish.rating, 'f', 1)));
    }

    if (m_desc) {
        m_desc->setText(m_dish.description.isEmpty() ? QStringLiteral("暂无描述") : m_dish.description);
    }

    if (m_price) {
        m_price->setText(QStringLiteral("¥%1").arg(QString::number(m_dish.price, 'f', 2)));
    }

    // 图片
    if (m_img) {
        QImage img;
        if (m_dish.url.startsWith(":/")) img = QImage(m_dish.url);
        if (!img.isNull()) m_img->setCardImage(img);
    }
}
