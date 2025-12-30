#include "dishinordercard.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ElaText.h"
#include "ElaImageCard.h"
#include "ElaPushButton.h"


DishInOrderCard::DishInOrderCard(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(100);
    setStyleSheet(
        "DishInOrderCard {"
        "background-color: rgba(255,255,255,0.95);"
        "border-radius: 8px;"
        "}"
    );

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(15, 10, 15, 10);
    mainLayout->setSpacing(15);

    // 左侧图片
    m_imageCard = new ElaImageCard(this);
    m_imageCard->setFixedSize(80, 80);
    m_imageCard->setBorderRadius(8);
    m_imageCard->setMaximumAspectRatio(1.0);
    m_imageCard->setIsPreserveAspectCrop(true);
    mainLayout->addWidget(m_imageCard);

    // 中间信息区域
    auto* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(8);

    m_nameLabel = new ElaText("", 16, this);
    m_nameLabel->setStyleSheet("font-weight: bold; color: #333333;");
    infoLayout->addWidget(m_nameLabel);

    m_quantityLabel = new ElaText("", 14, this);
    m_quantityLabel->setStyleSheet("color: #666666;");
    infoLayout->addWidget(m_quantityLabel);

    infoLayout->addStretch();
    mainLayout->addLayout(infoLayout, 1);

    // 右侧送达按钮
    m_servedBtn = new ElaPushButton(QStringLiteral("送达"), this);
    m_servedBtn->setFixedSize(80, 36);
    m_servedBtn->setLightDefaultColor(QColor("#4CAF50"));
    m_servedBtn->setDarkDefaultColor(QColor("#4CAF50"));
    connect(m_servedBtn, &ElaPushButton::clicked, this, [this]() {
        emit servedRequested(m_dishInOrder.dish.name);
    });
    mainLayout->addWidget(m_servedBtn, 0, Qt::AlignVCenter);
}

void DishInOrderCard::setDishInOrder(const DishInOrder& dishInOrder)
{
    m_dishInOrder = dishInOrder;
    refreshUI();
}

void DishInOrderCard::refreshUI()
{
    if (m_nameLabel) {
        m_nameLabel->setText(m_dishInOrder.dish.name);
    }

    if (m_quantityLabel) {
        m_quantityLabel->setText(QStringLiteral("数量：%1").arg(m_dishInOrder.quantity));
    }

    if (m_imageCard) {
        if (!m_dishInOrder.dish.url.isEmpty()) {
            QImage img;
            if (m_dishInOrder.dish.url.startsWith(":/")) {
                img = QImage(m_dishInOrder.dish.url);
            }
            if (!img.isNull()) {
                m_imageCard->setCardImage(img);
            }
        }
    }

    // 如果已送达，禁用按钮
    if (m_servedBtn) {
        m_servedBtn->setEnabled(!m_dishInOrder.isServed);
        if (m_dishInOrder.isServed) {
            m_servedBtn->setText(QStringLiteral("已送达"));
        }
    }
}
