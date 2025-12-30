#include "servicerequestcard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "ElaText.h"
#include "ElaPushButton.h"

ServiceRequestCard::ServiceRequestCard(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(120);
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

    // 第一行：客户名 + 状态
    auto* firstRow = new QHBoxLayout();
    firstRow->setSpacing(15);

    m_customerLabel = new ElaText("", 16, this);
    m_customerLabel->setStyleSheet("font-weight: bold; color: #333333;");
    firstRow->addWidget(m_customerLabel);

    m_statusLabel = new ElaText("", 14, this);
    firstRow->addWidget(m_statusLabel);
    firstRow->addStretch();

    infoLayout->addLayout(firstRow);

    // 第二行：时间
    m_timeLabel = new ElaText("", 12, this);
    m_timeLabel->setStyleSheet("color: #999999;");
    infoLayout->addWidget(m_timeLabel);

    mainLayout->addLayout(infoLayout, 1);

    // 右侧按钮区域
    auto* buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(8);
    buttonLayout->setAlignment(Qt::AlignVCenter);

    m_processBtn = new ElaPushButton(QStringLiteral("处理"), this);
    m_processBtn->setFixedSize(80, 32);
    m_processBtn->setLightDefaultColor(QColor("#2196F3"));
    m_processBtn->setDarkDefaultColor(QColor("#2196F3"));
    connect(m_processBtn, &ElaPushButton::clicked, this, [this]() {
        emit processRequested(m_request.request_id);
    });

    m_completeBtn = new ElaPushButton(QStringLiteral("完成"), this);
    m_completeBtn->setFixedSize(80, 32);
    m_completeBtn->setLightDefaultColor(QColor("#4CAF50"));
    m_completeBtn->setDarkDefaultColor(QColor("#4CAF50"));
    connect(m_completeBtn, &ElaPushButton::clicked, this, [this]() {
        emit completeRequested(m_request.request_id);
    });

    buttonLayout->addWidget(m_processBtn);
    buttonLayout->addWidget(m_completeBtn);

    mainLayout->addLayout(buttonLayout);
}

void ServiceRequestCard::setRequest(const ServiceRequest& request)
{
    m_request = request;
    refreshUI();
}

void ServiceRequestCard::refreshUI()
{
    if (m_customerLabel) {
        m_customerLabel->setText(QStringLiteral("客户：%1").arg(m_request.customer_name));
    }

    if (m_timeLabel) {
        m_timeLabel->setText(QStringLiteral("时间：%1").arg(m_request.create_time));
    }

    if (m_statusLabel) {
        QString statusText;
        QString statusColor;

        switch (m_request.status) {
        case ServiceRequestStatus::Pending:
            statusText = QStringLiteral("🕒 待完成");
            statusColor = "color: #FF9800; font-weight: bold;";
            break;
        case ServiceRequestStatus::Processing:
            statusText = QStringLiteral("⚙️ 正在处理");
            statusColor = "color: #2196F3; font-weight: bold;";
            break;
        }

        m_statusLabel->setText(statusText);
        m_statusLabel->setStyleSheet(statusColor);
    }

    // 根据状态控制按钮显示
    if (m_processBtn) {
        // 如果已经在处理中，禁用"处理"按钮
        m_processBtn->setEnabled(m_request.status == ServiceRequestStatus::Pending);
    }
}
