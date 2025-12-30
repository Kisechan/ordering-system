#include "ReviewDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

#include "ElaText.h"
#include "ElaPlainTextEdit.h"
#include "ElaScrollArea.h"
#include "ElaPushButton.h"

#include "RateDishCard.h"

ReviewDialog::ReviewDialog(int orderId,
                           double totalAmount,
                           const QString& time,
                           const QList<CartItem>& items,
                           const QString& presetComment,
                           QWidget* parent)
    : QDialog(parent),
      m_orderId(orderId),
      m_totalAmount(totalAmount),
      m_time(time),
      m_items(items)
{
    setWindowTitle(QStringLiteral("评价订单"));
    resize(600, 800);

    buildUI();

    if (m_edit) {
        m_edit->setPlainText(presetComment);
    }
}

QString ReviewDialog::comment() const
{
    return m_edit ? m_edit->toPlainText().trimmed() : QString();
}

QMap<int, int> ReviewDialog::ratings() const
{
    return m_ratings;
}

void ReviewDialog::buildUI()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    // ===== 1) 顶部订单信息卡片 =====
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

    // ===== 2) 评论区（也放进一个小卡片里更整齐）=====
    {
        auto* commentCard = new QWidget(this);
        commentCard->setObjectName("CommentCard");
        commentCard->setAttribute(Qt::WA_StyledBackground, true);
        commentCard->setStyleSheet(
            "#CommentCard {"
            "background-color:#ffffff;"
            "border:1px solid #d0d0d0;"
            "border-radius:12px;"
            "}"
        );

        auto* lay = new QVBoxLayout(commentCard);
        lay->setContentsMargins(16, 12, 16, 12);
        lay->setSpacing(8);

        auto* title = new ElaText(QStringLiteral("评论"), 17, commentCard);
        title->setStyleSheet("color:#222222;");
        lay->addWidget(title);

        m_edit = new ElaPlainTextEdit(commentCard);
        m_edit->setPlaceholderText(QStringLiteral("请输入评论（可留空）"));
        m_edit->setMinimumHeight(110);
        lay->addWidget(m_edit);

        root->addWidget(commentCard);
    }

    // ===== 3) 下方 scrollball：菜品评分列表 =====
    {
        m_scroll = new ElaScrollArea(this);
        m_scroll->setWidgetResizable(true);
        m_scroll->setAlignment(Qt::AlignTop);

        m_listContainer = new QWidget(m_scroll);
        m_listLayout = new QVBoxLayout(m_listContainer);
        m_listLayout->setContentsMargins(0, 0, 0, 0);
        m_listLayout->setSpacing(12);

        for (const auto& it : m_items) {
            auto* card = new RateDishCard(m_listContainer);
            card->setItem(it);

            // 默认 5 分（你也可以改成 3）
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
    }

    // ===== 4) 底部按钮 =====
    {
        auto* row = new QHBoxLayout();
        row->setContentsMargins(0, 0, 0, 0);

        auto* cancel = new ElaPushButton(QStringLiteral("取消"), this);
        cancel->setMinimumHeight(40);

        auto* ok = new ElaPushButton(QStringLiteral("提交"), this);
        ok->setMinimumHeight(40);

        row->addStretch(1);
        row->addWidget(cancel);
        row->addSpacing(10);
        row->addWidget(ok);

        root->addLayout(row);

        connect(cancel, &ElaPushButton::clicked, this, &QDialog::reject);
        connect(ok, &ElaPushButton::clicked, this, [this](){
            accept();
        });
    }
}

