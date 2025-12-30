#include "RateDishCard.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QImage>

#include "ElaImageCard.h"
#include "ElaText.h"

RateDishCard::RateDishCard(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("RateDishCardRoot");
    setStyleSheet(
        "#RateDishCardRoot {"
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

    // 右侧信息区
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

    m_qtyAndTotal = new ElaText("", 15, this);
    m_qtyAndTotal->setStyleSheet("color:#666666;");
    right->addWidget(m_qtyAndTotal);

    // 评分区：离散 1~5
    auto* rateRow = new QHBoxLayout();
    rateRow->setContentsMargins(0, 6, 0, 0);
    rateRow->setSpacing(10);

    m_rateText = new ElaText(QStringLiteral("评分：5"), 15, this);
    rateRow->addWidget(m_rateText);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(1, 5);
    m_slider->setSingleStep(1);
    m_slider->setPageStep(1);
    m_slider->setTickInterval(1);
    m_slider->setTickPosition(QSlider::TicksBelow);
    m_slider->setFixedWidth(180);
    m_slider->setValue(m_rating);
    rateRow->addWidget(m_slider);

    rateRow->addStretch(1);
    right->addLayout(rateRow);

    right->addStretch(1);
    root->addLayout(right, 1);

    connect(m_slider, &QSlider::valueChanged, this, &RateDishCard::onSliderChanged);

    setMinimumHeight(150);
    refreshUI();
}

void RateDishCard::setItem(const CartItem& item)
{
    m_item = item;
    refreshUI();
}

void RateDishCard::setRating(int rating)
{
    m_rating = qBound(1, rating, 5);
    if (m_slider) m_slider->setValue(m_rating);
    if (m_rateText) m_rateText->setText(QStringLiteral("评分：%1").arg(m_rating));
}

void RateDishCard::onSliderChanged(int v)
{
    m_rating = v;
    if (m_rateText) m_rateText->setText(QStringLiteral("评分：%1").arg(v));
    emit ratingChanged(m_item.dish.dish_id, v);
}

void RateDishCard::refreshUI()
{
    const Dish& d = m_item.dish;

    if (m_title) m_title->setText(d.name.isEmpty() ? QStringLiteral("未命名菜品") : d.name);

    if (m_meta) {
        m_meta->setText(QStringLiteral("品类：%1").arg(d.category.isEmpty() ? QStringLiteral("-") : d.category));
    }

    if (m_desc) m_desc->setText(d.description.isEmpty() ? QStringLiteral("（无描述）") : d.description);

    const double total = d.price * m_item.qty;
    if (m_qtyAndTotal) {
        m_qtyAndTotal->setText(QStringLiteral("数量：×%1    小计：¥ %2")
                               .arg(m_item.qty)
                               .arg(QString::number(total, 'f', 2)));
    }

    if (m_img) {
        QImage img;
        if (d.url.startsWith(":/")) img = QImage(d.url);
        if (!img.isNull()) m_img->setCardImage(img);
        else {
            img = QImage(":/Image/vvan.jpg");
            m_img->setCardImage(img);
        }
    }

    if (m_rateText) m_rateText->setText(QStringLiteral("评分：%1").arg(m_rating));
    if (m_slider) m_slider->setValue(m_rating);
}
