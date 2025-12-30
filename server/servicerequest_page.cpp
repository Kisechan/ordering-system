#include "servicerequest_page.h"
#include "servicerequestcard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QMessageBox>

#include "ElaLineEdit.h"
#include "ElaText.h"
#include "ElaPushButton.h"
#include "ElaScrollArea.h"
#include "ElaMessageBar.h"

ServiceRequest_Page::ServiceRequest_Page(QWidget* parent)
    : ElaScrollPage(parent)
{
    setTitleVisible(false);
    setPageTitleSpacing(0);

    // 主容器
    auto* mainContainer = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(20, 18, 20, 0);
    mainLayout->setSpacing(12);

    // 顶部操作栏（搜索框 + 刷新按钮）
    auto* topBar = new QWidget(mainContainer);
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(10);

    m_searchEdit = new ElaLineEdit(topBar);
    m_searchEdit->setPlaceholderText(QStringLiteral("搜索客户名"));
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumHeight(40);
    topLayout->addWidget(m_searchEdit, 1);

    m_refreshBtn = new ElaPushButton(QStringLiteral("刷新"), topBar);
    m_refreshBtn->setFixedSize(100, 40);
    QFont btnFont = m_refreshBtn->font();
    btnFont.setPixelSize(16);
    m_refreshBtn->setFont(btnFont);
    m_refreshBtn->setLightDefaultColor(QColor("#2196F3"));
    m_refreshBtn->setDarkDefaultColor(QColor("#2196F3"));
    topLayout->addWidget(m_refreshBtn);

    mainLayout->addWidget(topBar);

    // 滚动内容区域
    auto* scrollContent = new QWidget(mainContainer);
    auto* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 0, 0, 18);
    scrollLayout->setSpacing(12);

    // 卡片列表容器
    m_listContainer = new QWidget(scrollContent);
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(12);
    scrollLayout->addWidget(m_listContainer);

    scrollLayout->addStretch(1);
    mainLayout->addWidget(scrollContent, 1);

    addCentralWidget(mainContainer, true, true);

    if (auto* sa = this->findChild<ElaScrollArea*>()) {
        sa->setAlignment(Qt::AlignTop);
    }

    // 搜索防抖
    m_searchDebounce = new QTimer(this);
    m_searchDebounce->setSingleShot(true);
    m_searchDebounce->setInterval(200);

    connect(m_searchEdit, &ElaLineEdit::textChanged,
            this, &ServiceRequest_Page::onSearchTextChanged);
    connect(m_searchDebounce, &QTimer::timeout,
            this, &ServiceRequest_Page::applyFilterNow);
    connect(m_searchEdit, &ElaLineEdit::returnPressed,
            this, &ServiceRequest_Page::onSearchReturnPressed);
    connect(m_refreshBtn, &ElaPushButton::clicked,
            this, &ServiceRequest_Page::onRefresh);

    // 示例：
    QList<ServiceRequest> requests;

            ServiceRequest sr1;
            sr1.request_id = 1;
            sr1.customer_name = QStringLiteral("张三 - 8号桌");
            sr1.status = ServiceRequestStatus::Pending;
            sr1.create_time = QStringLiteral("2025-12-30 10:15:00");
            requests.append(sr1);

            ServiceRequest sr2;
            sr2.request_id = 2;
            sr2.customer_name = QStringLiteral("李四 - 3号桌");
            sr2.status = ServiceRequestStatus::Processing;
            sr2.create_time = QStringLiteral("2025-12-30 10:20:00");
            requests.append(sr2);

            ServiceRequest sr3;
            sr3.request_id = 3;
            sr3.customer_name = QStringLiteral("王五 - 12号桌");
            sr3.status = ServiceRequestStatus::Pending;
            sr3.create_time = QStringLiteral("2025-12-30 10:25:00");
            requests.append(sr3);

            ServiceRequest sr4;
            sr4.request_id = 4;
            sr4.customer_name = QStringLiteral("赵六 - 5号桌");
            sr4.status = ServiceRequestStatus::Pending;
            sr4.create_time = QStringLiteral("2025-12-30 10:30:00");
            requests.append(sr4);

    setRequestList(requests);

}

void ServiceRequest_Page::onSearchTextChanged(const QString& text)
{
    m_keyword = text.trimmed();
    m_searchDebounce->start();
}

void ServiceRequest_Page::setRequestList(const QList<ServiceRequest>& requests)
{
    m_allRequests = requests;
    applyFilterNow();
}

void ServiceRequest_Page::onSearchReturnPressed()
{
    emit searchRequested(m_keyword);
}

void ServiceRequest_Page::onRefresh()
{
    emit refreshRequested();
    ElaMessageBar::information(ElaMessageBarType::BottomRight,
                              QStringLiteral("提示"),
                              QStringLiteral("正在刷新服务请求列表..."),
                              1500, this);
}

void ServiceRequest_Page::applyFilterNow()
{
    QList<ServiceRequest> show;
    if (m_keyword.isEmpty()) {
        show = m_allRequests;
    } else {
        for (const auto& r : m_allRequests) {
            if (r.customer_name.contains(m_keyword, Qt::CaseInsensitive)) {
                show.push_back(r);
            }
        }
    }
    rebuildList(show);
}

void ServiceRequest_Page::rebuildList(const QList<ServiceRequest>& requests)
{
    if (!m_listLayout) return;

    // 清空旧卡片
    while (m_listLayout->count() > 0) {
        QLayoutItem* it = m_listLayout->takeAt(0);
        if (!it) break;
        if (QWidget* w = it->widget()) {
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete it;
    }

    if (requests.isEmpty()) {
        auto* empty = new ElaText(QStringLiteral("暂无服务请求"), 14, m_listContainer);
        empty->setStyleSheet("color:#888888;");
        empty->setAlignment(Qt::AlignHCenter);
        m_listLayout->addWidget(empty);
        return;
    }

    for (const auto& r : requests) {
        auto* card = new ServiceRequestCard(m_listContainer);
        card->setRequest(r);

        connect(card, &ServiceRequestCard::processRequested,
                this, &ServiceRequest_Page::onProcessRequest);
        connect(card, &ServiceRequestCard::completeRequested,
                this, &ServiceRequest_Page::onCompleteRequest);

        m_listLayout->addWidget(card);
    }
}

void ServiceRequest_Page::onProcessRequest(int requestId)
{
    // 查找对应的请求
    ServiceRequest* foundRequest = nullptr;
    for (auto& req : m_allRequests) {
        if (req.request_id == requestId) {
            foundRequest = &req;
            break;
        }
    }

    if (!foundRequest) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight,
                              QStringLiteral("提示"),
                              QStringLiteral("请求未找到"),
                              2000, this);
        return;
    }

    // 更新状态为正在处理
    foundRequest->status = ServiceRequestStatus::Processing;

    // 发射信号通知外部（用于数据库更新）
    emit requestProcessed(requestId);

    // 刷新显示
    applyFilterNow();

    ElaMessageBar::success(ElaMessageBarType::BottomRight,
                          QStringLiteral("成功"),
                          QStringLiteral("已标记为正在处理"),
                          2000, this);
}

void ServiceRequest_Page::onCompleteRequest(int requestId)
{
    // 查找对应的请求
    int index = -1;
    QString customerName;
    for (int i = 0; i < m_allRequests.size(); ++i) {
        if (m_allRequests[i].request_id == requestId) {
            index = i;
            customerName = m_allRequests[i].customer_name;
            break;
        }
    }

    if (index == -1) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight,
                              QStringLiteral("提示"),
                              QStringLiteral("请求未找到"),
                              2000, this);
        return;
    }

    // 显示确认对话框
    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle(QStringLiteral("确认完成"));
    confirmBox.setText(QStringLiteral("确定已完成 \"%1\" 的服务请求吗？").arg(customerName));
    confirmBox.setInformativeText(QStringLiteral("完成后该请求将被移除。"));
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::Yes);
    confirmBox.button(QMessageBox::Yes)->setText(QStringLiteral("确认"));
    confirmBox.button(QMessageBox::No)->setText(QStringLiteral("取消"));

    if (confirmBox.exec() == QMessageBox::Yes) {
        // 发射信号通知外部删除数据库记录
        emit requestCompleted(requestId);

        // 从本地列表中移除
        m_allRequests.removeAt(index);

        // 刷新显示
        applyFilterNow();

        ElaMessageBar::success(ElaMessageBarType::BottomRight,
                              QStringLiteral("成功"),
                              QStringLiteral("服务请求已完成"),
                              2000, this);
    }
}
