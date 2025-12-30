#include "RateDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "ElaScrollArea.h"
#include "ElaText.h"
#include "ElaPushButton.h"

#include "RateDishCard.h"

RateDialog::RateDialog(int orderId,
                       double totalAmount,
                       const QString& time,
                       const QList<CartItem>& items,
                       QWidget* parent)
    : QDialog(parent),
      m_orderId(orderId),
      m_totalAmount(totalAmount),
      m_time(time),
      m_items(items)
{
    setWindowTitle(QStringLiteral("订单评分"));
    resize(560, 560);
    buildUI();
}

void RateDialog::buildUI()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    // 顶部订单信息（信息卡片 + 价格数字橘色）
    {
        auto* infoCard = new QWidget(this);
        infoCard->setObjectName("OrderInfoCard");
        infoCard->setAttribute(Qt::WA_StyledBackground, true);
        infoCard->setStyleSheet(
            "#OrderInfoCard {"
            "background-color:#f7f7f7;"
            "border:1px solid #d0d0d0;"
            "border-radius:12px;"
            "}"
        );

        auto* lay = new QHBoxLayout(infoCard);
        lay->setContentsMargins(16, 12, 16, 12);
        lay->setSpacing(22);

        auto* t1 = new ElaText(QStringLiteral("订单ID：%1").arg(m_orderId), 18, infoCard);
        t1->setStyleSheet("color:#222222;");

        const QString priceText = QStringLiteral("¥ %1").arg(QString::number(m_totalAmount, 'f', 2));
        auto* t2 = new ElaText(
            QStringLiteral("总价：<span style='color:#ff6a00;'>%1</span>").arg(priceText),
            18,
            infoCard
        );
        t2->setTextFormat(Qt::RichText);
        t2->setStyleSheet("color:#222222;");

        auto* t3 = new ElaText(QStringLiteral("时间：%1").arg(m_time), 18, infoCard);
        t3->setStyleSheet("color:#222222;");

        lay->addWidget(t1);
        lay->addWidget(t2);
        lay->addStretch(1);
        lay->addWidget(t3);

        root->addWidget(infoCard);
    }



    // 下方 scrollball
    m_scroll = new ElaScrollArea(this);
    m_scroll->setWidgetResizable(true);
    m_scroll->setAlignment(Qt::AlignTop);

    m_listContainer = new QWidget(m_scroll);
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(12);

    // 构造卡片列表
    for (const auto& it : m_items) {
        auto* card = new RateDishCard(m_listContainer);
        card->setItem(it);

        // 默认先给 5 分（你也可以默认 3 分）
        m_ratings[it.dish.dish_id] = 5;

        connect(card, &RateDishCard::ratingChanged, this,
                [this](int dishId, int rating){
                    m_ratings[dishId] = rating;
                });

        m_listLayout->addWidget(card);
    }
    m_listLayout->addStretch(1);

    m_scroll->setWidget(m_listContainer);
    root->addWidget(m_scroll, 1);

    // 底部按钮（自适应宽度：用一行 layout + 两边 stretch）
    {
        auto* row = new QHBoxLayout();
        row->setContentsMargins(0, 0, 0, 0);

        auto* ok = new ElaPushButton(QStringLiteral("提交评分"), this);
        ok->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        ok->setMinimumHeight(40);

        row->addStretch(1);
        row->addWidget(ok);
        row->addStretch(1);

        root->addLayout(row);

        connect(ok, &ElaPushButton::clicked, this, [this](){
            accept();
        });
    }
}
