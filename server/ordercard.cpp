#include "ordercard.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

#include "ElaText.h"
#include "ElaPushButton.h"

OrderCard::OrderCard(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("OrderCardRoot");
    setStyleSheet(
        "#OrderCardRoot { "
        "background-color:#f7f7f7;"
        "border-radius:12px;"
        "border:1px solid #d0d0d0;"
        "}"
    );
    setAttribute(Qt::WA_StyledBackground, true);

    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(16, 14, 16, 14);
    root->setSpacing(16);

    // 订单号
    m_orderIdText = new ElaText("", 16, this);
    m_orderIdText->setFixedWidth(100);
    root->addWidget(m_orderIdText);

    // 用户名
    m_userNameText = new ElaText("", 16, this);
    m_userNameText->setFixedWidth(120);
    root->addWidget(m_userNameText);

    // 总金额
    m_totalAmountText = new ElaText("", 18, this);
    m_totalAmountText->setStyleSheet("color:#ff6a00; font-weight: bold;");
    m_totalAmountText->setFixedWidth(100);
    root->addWidget(m_totalAmountText);

    // 下单时间
    m_createTimeText = new ElaText("", 15, this);
    m_createTimeText->setStyleSheet("color:#666666;");
    m_createTimeText->setFixedWidth(150);
    root->addWidget(m_createTimeText);

    // 评论
    m_commentText = new ElaText("", 14, this);
    m_commentText->setStyleSheet("color:#888888;");
    root->addWidget(m_commentText, 1);

    // 查看详情按钮
    m_detailBtn = new ElaPushButton(QStringLiteral("查看详情"), this);
    QFont f = m_detailBtn->font();
    f.setPixelSize(15);
    m_detailBtn->setFont(f);
    m_detailBtn->setMinimumHeight(32);
    m_detailBtn->setFixedWidth(100);
    m_detailBtn->setStyleSheet(
        "ElaPushButton { padding: 6px 12px; }"
    );
    root->addWidget(m_detailBtn);

    connect(m_detailBtn, &ElaPushButton::clicked, this, &OrderCard::onViewDetail);

    setMinimumHeight(80);
    refreshUI();
}

void OrderCard::setOrder(const Order& order)
{
    m_order = order;
    refreshUI();
}

void OrderCard::onViewDetail()
{
    emit viewDetailRequested(m_order.order_id);
}

void OrderCard::refreshUI()
{
    if (m_orderIdText) {
        m_orderIdText->setText(QStringLiteral("#%1").arg(m_order.order_id));
    }

    if (m_userNameText) {
        m_userNameText->setText(m_order.user_name.isEmpty() ? QStringLiteral("-") : m_order.user_name);
    }

    if (m_totalAmountText) {
        m_totalAmountText->setText(QStringLiteral("¥%1").arg(QString::number(m_order.total_amount, 'f', 2)));
    }

    if (m_createTimeText) {
        if (m_order.create_time.isValid()) {
            m_createTimeText->setText(m_order.create_time.toString("yyyy-MM-dd hh:mm"));
        } else {
            m_createTimeText->setText("-");
        }
    }

    if (m_commentText) {
        QString fullComment = m_order.comment.isEmpty() ? QStringLiteral("无备注") : m_order.comment;

        // 设置字数限制，超过20个字符用省略号
        const int maxLength = 20;
        QString displayText = fullComment;

        if (fullComment.length() > maxLength) {
            displayText = fullComment.left(maxLength) + "...";
            // 设置toolTip显示完整内容
            m_commentText->setToolTip(fullComment);
        } else {
            m_commentText->setToolTip(""); // 清除tooltip
        }

        m_commentText->setText(displayText);
    }
}
