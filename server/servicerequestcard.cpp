#include "servicerequestcard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "ElaText.h"
#include "ElaPushButton.h"

ServiceRequestCard::ServiceRequestCard(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(150);
    setStyleSheet(
        "ServiceRequestCard {"
        "background-color: rgba(255,255,255,0.9);"
        "border-radius: 10px;"
        "}"
    );

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(15);

    // 左侧信息区域
    auto* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(8);

    // 第一行：桌号 + 状态
    auto* firstRow = new QHBoxLayout();
    firstRow->setSpacing(15);

    m_tableLabel = new ElaText("", 18, this);
    m_tableLabel->setStyleSheet("font-weight: bold; color: #333333;");
    firstRow->addWidget(m_tableLabel);

    m_statusLabel = new ElaText("", 14, this);
    firstRow->addWidget(m_statusLabel);
    firstRow->addStretch();

    infoLayout->addLayout(firstRow);

    // 第二行：呼叫提示（动态显示）
    m_callLabel = new ElaText(QStringLiteral("📞 顾客呼叫服务员"), 14, this);
    m_callLabel->setStyleSheet("color: #FF5722; font-weight: bold;");
    m_callLabel->setVisible(false);
    infoLayout->addWidget(m_callLabel);

    mainLayout->addLayout(infoLayout, 1);

    // 右侧按钮区域
    auto* buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(8);
    buttonLayout->setAlignment(Qt::AlignVCenter);

    m_serveBtn = new ElaPushButton(QStringLiteral("上菜"), this);
    m_serveBtn->setFixedSize(80, 32);
    m_serveBtn->setLightDefaultColor(QColor("#2196F3"));
    m_serveBtn->setDarkDefaultColor(QColor("#2196F3"));
    connect(m_serveBtn, &ElaPushButton::clicked, this, [this]() {
        emit serveRequested(m_table.table_number);
    });

    m_handleCallBtn = new ElaPushButton(QStringLiteral("处理"), this);
    m_handleCallBtn->setFixedSize(80, 32);
    m_handleCallBtn->setLightDefaultColor(QColor("#FF9800"));
    m_handleCallBtn->setDarkDefaultColor(QColor("#FF9800"));
    m_handleCallBtn->setVisible(false);
    connect(m_handleCallBtn, &ElaPushButton::clicked, this, [this]() {
        emit handleCallRequested(m_table.table_number);
    });

    m_completeBtn = new ElaPushButton(QStringLiteral("完成"), this);
    m_completeBtn->setFixedSize(80, 32);
    m_completeBtn->setLightDefaultColor(QColor("#4CAF50"));
    m_completeBtn->setDarkDefaultColor(QColor("#4CAF50"));
    connect(m_completeBtn, &ElaPushButton::clicked, this, [this]() {
        emit completeRequested(m_table.table_number);
    });

    buttonLayout->addWidget(m_serveBtn);
    buttonLayout->addWidget(m_handleCallBtn);
    buttonLayout->addWidget(m_completeBtn);

    mainLayout->addLayout(buttonLayout);
}

void ServiceRequestCard::setTableInfo(const TableInfo& table)
{
    m_table = table;
    refreshUI();
}

void ServiceRequestCard::refreshUI()
{
    if (m_tableLabel) {
        m_tableLabel->setText(QStringLiteral("%1号桌").arg(m_table.table_number));
    }

    if (m_statusLabel) {
        QString statusText;
        QString statusColor;

        switch (m_table.status) {
        case TableServiceStatus::Pending:
            statusText = QStringLiteral("🕒 待处理");
            statusColor = "color: #FF9800; font-weight: bold;";
            break;
        case TableServiceStatus::Processing:
            statusText = QStringLiteral("⚙️ 正在处理");
            statusColor = "color: #2196F3; font-weight: bold;";
            break;
        case TableServiceStatus::Completed:
            statusText = QStringLiteral("✅ 已完成");
            statusColor = "color: #4CAF50; font-weight: bold;";
            break;
        }

        m_statusLabel->setText(statusText);
        m_statusLabel->setStyleSheet(statusColor);
    }

    // 呼叫状态显示
    if (m_callLabel) {
        m_callLabel->setVisible(m_table.has_call);
    }

    if (m_handleCallBtn) {
        m_handleCallBtn->setVisible(m_table.has_call);
    }

    // 完成按钮只有在已完成状态才能点击
    if (m_completeBtn) {
        m_completeBtn->setEnabled(m_table.status == TableServiceStatus::Completed);
    }
}
