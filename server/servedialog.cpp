#include "servedialog.h"
#include "dishinordercard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QShowEvent>
#include <QPushButton>

#include "ElaText.h"
#include "ElaPushButton.h"
#include "ElaMessageBar.h"

ServeDialog::ServeDialog(QWidget* parent)
    : ElaContentDialog(parent)
{
    setWindowTitle(QStringLiteral("上菜"));
    setWindowModality(Qt::ApplicationModal);
    setMinimumSize(700, 600);

    auto* content = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 标题
    m_titleLabel = new ElaText(QStringLiteral("上菜管理"), 20, content);
    m_titleLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(m_titleLabel);

    // 菜品列表容器
    m_dishListContainer = new QWidget(content);
    m_dishListLayout = new QVBoxLayout(m_dishListContainer);
    m_dishListLayout->setContentsMargins(0, 0, 0, 0);
    m_dishListLayout->setSpacing(12);

    mainLayout->addWidget(m_dishListContainer, 1);

    // 底部关闭按钮
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 10, 0, 0);
    buttonLayout->addStretch();

    auto* closeBtn = new ElaPushButton(QStringLiteral("关闭"), content);
    closeBtn->setFixedSize(100, 36);
    QFont btnFont = closeBtn->font();
    btnFont.setPixelSize(15);
    closeBtn->setFont(btnFont);
    connect(closeBtn, &ElaPushButton::clicked, this, &ServeDialog::accept);

    buttonLayout->addWidget(closeBtn);
    mainLayout->addLayout(buttonLayout);

    setCentralWidget(content);
}

void ServeDialog::setTableInfo(const TableInfo& table)
{
    m_table = table;

    if (m_titleLabel) {
        m_titleLabel->setText(QStringLiteral("%1号桌 - 上菜管理").arg(m_table.table_number));
    }

    rebuildDishList();
}

void ServeDialog::rebuildDishList()
{
    if (!m_dishListLayout) return;

    // 清空旧内容
    while (m_dishListLayout->count() > 0) {
        QLayoutItem* it = m_dishListLayout->takeAt(0);
        if (!it) break;
        if (QWidget* w = it->widget()) {
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete it;
    }

    // 统计未送达的菜品数量
    int unservedCount = 0;
    for (const auto& dish : m_table.dishes) {
        if (!dish.isServed) {
            unservedCount++;
        }
    }

    if (unservedCount == 0) {
        auto* empty = new ElaText(QStringLiteral("✅ 所有菜品已送达！"), 16, m_dishListContainer);
        empty->setStyleSheet("color: #4CAF50; font-weight: bold;");
        empty->setAlignment(Qt::AlignHCenter);
        m_dishListLayout->addWidget(empty);
        return;
    }

    // 添加菜品卡片（只显示未送达的）
    for (const auto& dish : m_table.dishes) {
        if (!dish.isServed) {
            auto* card = new DishInOrderCard(m_dishListContainer);
            card->setDishInOrder(dish);

            connect(card, &DishInOrderCard::servedRequested,
                    this, &ServeDialog::onDishServed);

            m_dishListLayout->addWidget(card);
        }
    }
}

void ServeDialog::onDishServed(const QString& dishName)
{
    // 标记菜品为已送达
    bool found = false;
    for (auto& dish : m_table.dishes) {
        if (dish.dish.name == dishName && !dish.isServed) {
            dish.isServed = true;
            found = true;
            break;
        }
    }

    if (found) {
        // 发射信号通知外部
        emit dishServed(m_table.table_number, dishName);

        // 刷新显示
        rebuildDishList();

        // 检查是否全部送达
        bool allServed = true;
        for (const auto& dish : m_table.dishes) {
            if (!dish.isServed) {
                allServed = false;
                break;
            }
        }

        if (allServed) {
            ElaMessageBar::success(ElaMessageBarType::BottomRight,
                                  QStringLiteral("成功"),
                                  QStringLiteral("所有菜品已送达，可以点击完成按钮！"),
                                  2000, this);
        } else {
            ElaMessageBar::success(ElaMessageBarType::BottomRight,
                                  QStringLiteral("成功"),
                                  QStringLiteral("菜品已送达"),
                                  1500, this);
        }
    }
}

void ServeDialog::showEvent(QShowEvent* event)
{
    ElaContentDialog::showEvent(event);

    // 在对话框显示后，查找并隐藏底部的三个按钮
    QList<QPushButton*> buttons = this->findChildren<QPushButton*>();
    for (QPushButton* btn : buttons) {
        // 通过对象名或文本特征识别底部按钮并隐藏
        QString text = btn->text().toLower();
        if (text.contains("cancel") || text.contains("minimum") || text.contains("exit") ||
            text.contains("取消") || text.contains("最小化") || text.contains("退出")) {
            btn->hide();
        }
    }
}
