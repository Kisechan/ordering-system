#include "orderdetaildialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QVariant>

#include "ElaText.h"
#include "ElaScrollArea.h"
#include "ElaImageCard.h"
#include "ElaPushButton.h"
#include "dishcard.h"

OrderDetailDialog::OrderDetailDialog(QWidget* parent)
    : ElaContentDialog(parent)
{
    setWindowTitle(QStringLiteral("订单详情"));
    setWindowModality(Qt::ApplicationModal);


    // 设置对话框大小
    setMinimumSize(800, 600);

    auto* content = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 订单信息区域
    auto* infoWidget = new QWidget(content);
    infoWidget->setObjectName("OrderInfoCard");
    infoWidget->setStyleSheet(
        "#OrderInfoCard { "
        "background-color:#f7f7f7;"
        "border-radius:12px;"
        "border:1px solid #d0d0d0;"
        "}"
    );
    infoWidget->setAttribute(Qt::WA_StyledBackground, true);

    auto* infoLayout = new QHBoxLayout(infoWidget);
    infoLayout->setContentsMargins(20, 16, 20, 16);
    infoLayout->setSpacing(30);

    // 左侧：订单基本信息
    auto* leftInfo = new QVBoxLayout();
    leftInfo->setSpacing(10);

    m_orderIdLabel = new ElaText("", 20, infoWidget);
    m_orderIdLabel->setStyleSheet("font-weight: bold;");

    m_userLabel = new ElaText("", 16, infoWidget);
    m_userLabel->setStyleSheet("color:#333333;");

    m_timeLabel = new ElaText("", 15, infoWidget);
    m_timeLabel->setStyleSheet("color:#666666;");

    leftInfo->addWidget(m_orderIdLabel);
    leftInfo->addWidget(m_userLabel);
    leftInfo->addWidget(m_timeLabel);
    leftInfo->addStretch();

    // 右侧：评价和总金额
    auto* rightInfo = new QVBoxLayout();
    rightInfo->setSpacing(10);

    m_commentLabel = new ElaText("", 15, infoWidget);
    m_commentLabel->setStyleSheet("color:#666666;");
    m_commentLabel->setWordWrap(true);

    m_totalLabel = new ElaText("", 24, infoWidget);
    m_totalLabel->setStyleSheet("color:#ff6a00; font-weight: bold;");

    rightInfo->addWidget(m_commentLabel);
    rightInfo->addStretch();
    rightInfo->addWidget(m_totalLabel);

    infoLayout->addLayout(leftInfo, 1);
    infoLayout->addLayout(rightInfo, 1);

    mainLayout->addWidget(infoWidget);

    // 菜品列表标题
    auto* dishTitle = new ElaText(QStringLiteral("订单菜品："), 17, content);
    dishTitle->setStyleSheet("font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(dishTitle);

    // 滚动区域
    auto* scrollArea = new ElaScrollArea(content);
    scrollArea->setWidgetResizable(true);

    m_dishListContainer = new QWidget();
    m_dishListLayout = new QVBoxLayout(m_dishListContainer);
    m_dishListLayout->setContentsMargins(0, 0, 0, 0);
    m_dishListLayout->setSpacing(12);
    m_dishListLayout->addStretch();

    scrollArea->setWidget(m_dishListContainer);
    mainLayout->addWidget(scrollArea, 1);

    // 底部按钮区域
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 10, 0, 0);
    buttonLayout->addStretch();

    auto* closeBtn = new ElaPushButton(QStringLiteral("关闭"), content);
    closeBtn->setFixedSize(100, 36);
    QFont btnFont = closeBtn->font();
    btnFont.setPixelSize(15);
    closeBtn->setFont(btnFont);
    connect(closeBtn, &ElaPushButton::clicked, this, &OrderDetailDialog::reject);

    buttonLayout->addWidget(closeBtn);
    mainLayout->addLayout(buttonLayout);

    // 设置对话框的中央内容
    setCentralWidget(content);
}

void OrderDetailDialog::setOrder(const Order& order)
{
    m_order = order;

    // 更新订单信息
    if (m_orderIdLabel) {
        m_orderIdLabel->setText(QStringLiteral("订单号：#%1").arg(m_order.order_id));
    }
    if (m_userLabel) {
        m_userLabel->setText(QStringLiteral("用户：%1 (ID: %2)")
                          .arg(m_order.user_name.isEmpty() ? QStringLiteral("未知") : m_order.user_name)
                          .arg(m_order.user_id));
    }
    if (m_timeLabel) {
        if (m_order.create_time.isValid()) {
            m_timeLabel->setText(QStringLiteral("下单时间：%1").arg(m_order.create_time.toString("yyyy-MM-dd hh:mm:ss")));
        } else {
            m_timeLabel->setText(QStringLiteral("下单时间：-"));
        }
    }
    if (m_commentLabel) {
        QString fullComment = m_order.comment.isEmpty() ? QStringLiteral("无") : m_order.comment;
        QString displayText;

        // 设置字数限制，超过50个字符用省略号
        const int maxLength = 50;

        if (fullComment.length() > maxLength) {
            displayText = QStringLiteral("评论：%1...").arg(fullComment.left(maxLength));
            // 设置toolTip显示完整内容
            m_commentLabel->setToolTip(QStringLiteral("评论：%1").arg(fullComment));
        } else {
            displayText = QStringLiteral("评论：%1").arg(fullComment);
            m_commentLabel->setToolTip(""); // 清除tooltip
        }

        m_commentLabel->setText(displayText);
    }
    if (m_totalLabel) {
        m_totalLabel->setText(QStringLiteral("总金额：¥%1").arg(QString::number(m_order.total_amount, 'f', 2)));
    }

    buildDishList();
}

void OrderDetailDialog::buildDishList()
{
    if (!m_dishListLayout) return;

    // 清空旧的菜品卡片
    while (m_dishListLayout->count() > 0) {
        QLayoutItem* item = m_dishListLayout->takeAt(0);
        if (!item) break;
        if (QWidget* w = item->widget()) {
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete item;
    }

    if (m_order.dishes.isEmpty()) {
        auto* emptyLabel = new ElaText(QStringLiteral("该订单暂无菜品"), 14, m_dishListContainer);
        emptyLabel->setStyleSheet("color:#888888;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        m_dishListLayout->addWidget(emptyLabel);
        m_dishListLayout->addStretch();
        return;
    }

    // 添加菜品卡片
    for (const auto& orderDish : m_order.dishes) {
        const Dish& dish = orderDish.dish;
        int quantity = orderDish.quantity;
        double customerRating = orderDish.customer_rating;
        double totalPrice = dish.price * quantity;

        auto* dishRow = new QWidget(m_dishListContainer);
        dishRow->setObjectName("DishRowInOrder");
        dishRow->setStyleSheet(
            "#DishRowInOrder { "
            "background-color:#f7f7f7;"
            "border-radius:10px;"
            "border:1px solid #d0d0d0;"
            "}"
        );
        dishRow->setAttribute(Qt::WA_StyledBackground, true);
        dishRow->setMinimumHeight(130);

        auto* rowLayout = new QHBoxLayout(dishRow);
        rowLayout->setContentsMargins(14, 12, 14, 12);
        rowLayout->setSpacing(16);

        // 左侧图片
        auto* dishImage = new ElaImageCard(dishRow);
        dishImage->setFixedSize(160, 106);
        dishImage->setBorderRadius(8);
        dishImage->setMaximumAspectRatio(1.0);
        dishImage->setIsPreserveAspectCrop(true);

        // 设置图片
        if (!dish.url.isEmpty()) {
            QImage img;
            if (dish.url.startsWith(":/")) {
                img = QImage(dish.url);
            }
            if (!img.isNull()) {
                dishImage->setCardImage(img);
            }
        }

        rowLayout->addWidget(dishImage, 0, Qt::AlignTop);

        // 右侧信息
        auto* rightLayout = new QVBoxLayout();
        rightLayout->setContentsMargins(0, 0, 0, 0);
        rightLayout->setSpacing(8);

        auto* nameLabel = new ElaText(dish.name, 18, dishRow);
        nameLabel->setStyleSheet("font-weight: bold;");

        // 显示品类和客户评分（注：不是总体评分）
        auto* metaLabel = new ElaText(
            QStringLiteral("品类：%1    顾客的评分：%2")
                .arg(dish.category.isEmpty() ? QStringLiteral("-") : dish.category)
                .arg(customerRating > 0 ? QString::number(customerRating, 'f', 1) : QStringLiteral("未评分")),
            15, dishRow);
        metaLabel->setStyleSheet("color:#666666;");

        auto* descLabel = new ElaText(dish.description, 14, dishRow);
        descLabel->setStyleSheet("color:#888888;");
        descLabel->setWordWrap(true);

        // 底部价格区域：显示数量、单价和总价
        auto* priceLayout = new QHBoxLayout();
        priceLayout->setSpacing(15);

        auto* unitPriceLabel = new ElaText(
            QStringLiteral("单价：¥%1").arg(QString::number(dish.price, 'f', 2)),
            15, dishRow);
        unitPriceLabel->setStyleSheet("color:#888888;");

        auto* quantityLabel = new ElaText(
            QStringLiteral("× %1").arg(quantity),
            16, dishRow);
        quantityLabel->setStyleSheet("color:#666666; font-weight: bold;");

        auto* totalPriceLabel = new ElaText(
            QStringLiteral("小计：¥%1").arg(QString::number(totalPrice, 'f', 2)),
            20, dishRow);
        totalPriceLabel->setStyleSheet("color:#ff6a00; font-weight: bold;");

        priceLayout->addWidget(unitPriceLabel);
        priceLayout->addWidget(quantityLabel);
        priceLayout->addStretch();
        priceLayout->addWidget(totalPriceLabel);

        rightLayout->addWidget(nameLabel);
        rightLayout->addWidget(metaLabel);
        rightLayout->addWidget(descLabel);
        rightLayout->addStretch();
        rightLayout->addLayout(priceLayout);

        rowLayout->addLayout(rightLayout, 1);

        m_dishListLayout->addWidget(dishRow);
    }

    m_dishListLayout->addStretch();
}
