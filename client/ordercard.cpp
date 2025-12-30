#include "OrderCard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "ElaText.h"
#include "ElaPushButton.h"

OrderCard::OrderCard(QWidget* parent)
    : QWidget(parent)
{
    QFont f;

    setObjectName("OrderCardRoot");
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "#OrderCardRoot {"
        " background-color:#f7f7f7;"
        " border-radius:12px;"
        " border:1px solid #d0d0d0;"
        "}"
    );

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 14, 16, 14);
    root->setSpacing(10);

    // 第一行：订单号 + 时间
    auto* row1 = new QHBoxLayout();
    row1->setSpacing(10);

    m_title = new ElaText("", 18, this);
    row1->addWidget(m_title, 1);

    m_time = new ElaText("", 14, this);
    m_time->setStyleSheet("color:#666666;");
    row1->addWidget(m_time, 0, Qt::AlignRight);

    root->addLayout(row1);

    // 第二行：评论（可隐藏）
    m_commentText = new ElaText("", 14, this);
    m_commentText->setStyleSheet("color:#444444;");
    root->addWidget(m_commentText);

    // 第三行：总价 + 两个按钮
    auto* row3 = new QHBoxLayout();
    row3->setSpacing(10);

    m_price = new ElaText("", 20, this);
    m_price->setStyleSheet("color:#ff6a00;");
    row3->addWidget(m_price);

    row3->addStretch(1);

    m_rateBtn = new ElaPushButton(QStringLiteral("评价"), this);
    f = m_rateBtn->font();
    f.setPixelSize(16);
    m_rateBtn->setFont(f);
    m_rateBtn->setMinimumHeight(32);
    m_rateBtn->setStyleSheet("ElaPushButton { padding: 6px 14px; }");
    row3->addWidget(m_rateBtn);

    root->addLayout(row3);

    connect(m_rateBtn, &ElaPushButton::clicked, this, &OrderCard::onRate);
}

void OrderCard::setOrder(int orderId,
                         double totalAmount,
                         const QString& createTime,
                         const QString& comment,
                         const QList<CartItem>& items)
{
    m_orderId = orderId;
    m_totalAmount = totalAmount;
    m_createTime = createTime;
    m_comment = comment;
    m_items = items;
    refreshUI();
}

void OrderCard::refreshUI()
{
    if (m_title) m_title->setText(QStringLiteral("订单 #%1").arg(m_orderId));
    if (m_time)  m_time->setText(m_createTime);

    if (m_price) m_price->setText(QStringLiteral("¥ %1").arg(QString::number(m_totalAmount, 'f', 2)));

    const QString c = m_comment.trimmed();
    if (m_commentText) {
        if (c.isEmpty()) {
            m_commentText->setVisible(false); // 没评论就不显示
        } else {
            m_commentText->setVisible(true);
            m_commentText->setText(QStringLiteral("评语：%1").arg(c));
        }
    }
}

void OrderCard::onRate()
{
    emit rateRequested(m_orderId);
}
