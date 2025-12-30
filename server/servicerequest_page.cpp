#include "servicerequest_page.h"
#include "servicerequestcard.h"
#include "servedialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QMessageBox>
#include <QRandomGenerator>

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

    // 顶部操作栏（只有刷新按钮）
    auto* topBar = new QWidget(mainContainer);
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(10);

    auto* titleLabel = new ElaText(QStringLiteral("桌号服务管理"), 18, topBar);
    titleLabel->setStyleSheet("font-weight: bold;");
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

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

    connect(m_refreshBtn, &ElaPushButton::clicked,
            this, &ServiceRequest_Page::onRefresh);

    // 初始化10个桌号
    initializeTables();

    // ===== 测试数据 =====
    Dish testDish1;
    testDish1.dish_id = 1;
    testDish1.name = QStringLiteral("宫保鸡丁");
    testDish1.price = 28.00;
    testDish1.url = QStringLiteral(":/Image/vvan.jpg");

    Dish testDish2;
    testDish2.dish_id = 2;
    testDish2.name = QStringLiteral("鱼香肉丝");
    testDish2.price = 32.00;
    testDish2.url = QStringLiteral(":/Image/vvan.jpg");

    // 3号桌测试数据
    for (auto& table : m_allTables) {
        if (table.table_number == 3) {
            table.is_idle = false;
            table.order_id = 5001;
            table.status = TableServiceStatus::Pending;

            DishInOrder dio1;
            dio1.dish = testDish1;
            dio1.quantity = 2;
            dio1.isServed = false;
            table.dishes.append(dio1);

            DishInOrder dio2;
            dio2.dish = testDish2;
            dio2.quantity = 1;
            dio2.isServed = false;
            table.dishes.append(dio2);
            break;
        }
    }

    // 5号桌测试数据（正在处理，有呼叫）
    for (auto& table : m_allTables) {
        if (table.table_number == 5) {
            table.is_idle = false;
            table.order_id = 5002;
            table.status = TableServiceStatus::Processing;
            table.has_call = true;

            DishInOrder dio;
            dio.dish = testDish1;
            dio.quantity = 1;
            dio.isServed = false;
            table.dishes.append(dio);
            break;
        }
    }

    rebuildList(m_allTables);

}

void ServiceRequest_Page::setTableList(const QList<TableInfo>& tables)
{
    m_allTables = tables;
    rebuildList(m_allTables);
}

void ServiceRequest_Page::initializeTables()
{
    m_allTables.clear();

    // 初始化10个桌号，全部为空闲状态
    for (int i = 1; i <= 10; ++i) {
        TableInfo table;
        table.table_number = i;
        table.customer_id = 0;
        table.order_id = 0;
        table.status = TableServiceStatus::Pending;
        table.is_idle = true;  // 初始全部空闲
        table.has_call = false;
        m_allTables.append(table);
    }

    rebuildList(m_allTables);
}

int ServiceRequest_Page::assignTable()
{
    // 查找空闲桌号
    QList<int> idleTables;
    for (const auto& table : m_allTables) {
        if (table.is_idle) {
            idleTables.append(table.table_number);
        }
    }

    if (idleTables.isEmpty()) {
        return -1;  // 无空闲桌号
    }

    // 随机选择一个空闲桌号
    int randomIndex = QRandomGenerator::global()->bounded(idleTables.size());
    return idleTables[randomIndex];
}

void ServiceRequest_Page::addNewOrder(int tableNumber, int customerId, int orderId, const QList<DishInOrder>& dishes)
{
    // 查找对应的桌号（桌号已经在初始化时创建）
    TableInfo* foundTable = nullptr;
    for (auto& table : m_allTables) {
        if (table.table_number == tableNumber) {
            foundTable = &table;
            break;
        }
    }

    if (!foundTable) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight,
                              QStringLiteral("错误"),
                              QStringLiteral("桌号 %1 不存在").arg(tableNumber),
                              2000, this);
        return;
    }

    // 更新桌号信息
    foundTable->order_id = orderId;
    foundTable->customer_id = customerId;
    foundTable->is_idle = false;  // 设置为非空闲

    // 根据当前状态决定是否改变状态
    if (foundTable->status == TableServiceStatus::Completed) {
        foundTable->status = TableServiceStatus::Pending;
    }

    // 添加新菜品到队列
    for (const auto& dish : dishes) {
        foundTable->dishes.append(dish);
    }

    rebuildList(m_allTables);

    ElaMessageBar::information(ElaMessageBarType::BottomRight,
                              QStringLiteral("新订单"),
                              QStringLiteral("%1号桌有新订单").arg(tableNumber),
                              2000, this);
}

void ServiceRequest_Page::setTableCall(int tableNumber, bool hasCall)
{
    for (auto& table : m_allTables) {
        if (table.table_number == tableNumber) {
            table.has_call = hasCall;
            rebuildList(m_allTables);
            return;
        }
    }
}

void ServiceRequest_Page::onRefresh()
{

    // 遍历所有非空闲桌号，根据order_id从数据库查询菜品
    rebuildList(m_allTables);

    emit refreshRequested();
    ElaMessageBar::information(ElaMessageBarType::BottomRight,
                              QStringLiteral("提示"),
                              QStringLiteral("正在刷新桌号服务列表..."),
                              1500, this);
}

void ServiceRequest_Page::setCardsEnabled(bool enabled)
{
    if (!m_listLayout) return;

    // 启用/禁用所有卡片
    for (int i = 0; i < m_listLayout->count(); ++i) {
        if (auto* card = qobject_cast<ServiceRequestCard*>(m_listLayout->itemAt(i)->widget())) {
            card->setEnabled(enabled);
        }
    }

    // 同时禁用刷新按钮
    if (m_refreshBtn) {
        m_refreshBtn->setEnabled(enabled);
    }
}

void ServiceRequest_Page::rebuildList(const QList<TableInfo>& tables)
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

    // 只显示非空闲的桌号
    QList<TableInfo> activeTables;
    for (const auto& table : tables) {
        if (!table.is_idle) {
            activeTables.append(table);
        }
    }

    if (activeTables.isEmpty()) {
        auto* empty = new ElaText(QStringLiteral("暂无服务请求"), 14, m_listContainer);
        empty->setStyleSheet("color:#888888;");
        empty->setAlignment(Qt::AlignHCenter);
        m_listLayout->addWidget(empty);
        return;
    }

    for (const auto& table : activeTables) {
        auto* card = new ServiceRequestCard(m_listContainer);
        card->setTableInfo(table);

        connect(card, &ServiceRequestCard::serveRequested,
                this, &ServiceRequest_Page::onServeTable);
        connect(card, &ServiceRequestCard::completeRequested,
                this, &ServiceRequest_Page::onCompleteTable);
        connect(card, &ServiceRequestCard::handleCallRequested,
                this, &ServiceRequest_Page::onHandleCall);

        m_listLayout->addWidget(card);
    }
}

void ServiceRequest_Page::onServeTable(int tableNumber)
{
    // 查找对应桌号
    TableInfo* foundTable = nullptr;
    for (auto& table : m_allTables) {
        if (table.table_number == tableNumber) {
            foundTable = &table;
            break;
        }
    }

    if (!foundTable) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight,
                              QStringLiteral("提示"),
                              QStringLiteral("桌号未找到"),
                              2000, this);
        return;
    }

    // 打开上菜对话框
    auto* dialog = new ServeDialog(this);
    dialog->setTableInfo(*foundTable);

    connect(dialog, &ServeDialog::dishServed,
            this, &ServiceRequest_Page::onDishServed);

    dialog->exec();

    // 对话框关闭后，更新桌号信息
    *foundTable = dialog->getTableInfo();

    // 检查是否所有菜品已送达
    bool allServed = true;
    for (const auto& dish : foundTable->dishes) {
        if (!dish.isServed) {
            allServed = false;
            break;
        }
    }

    if (allServed) {
        foundTable->status = TableServiceStatus::Completed;
    } else {
        // 只要有菜品送达，状态就变为正在处理
        bool anyServed = false;
        for (const auto& dish : foundTable->dishes) {
            if (dish.isServed) {
                anyServed = true;
                break;
            }
        }
        if (anyServed) {
            foundTable->status = TableServiceStatus::Processing;
        }
    }

    rebuildList(m_allTables);
    dialog->deleteLater();
}

void ServiceRequest_Page::onCompleteTable(int tableNumber)
{
    // 查找对应桌号
    int index = -1;
    for (int i = 0; i < m_allTables.size(); ++i) {
        if (m_allTables[i].table_number == tableNumber) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight,
                              QStringLiteral("提示"),
                              QStringLiteral("桌号未找到"),
                              2000, this);
        return;
    }

    // 确认对话框（使用堆分配避免生命周期问题）
    auto* confirmBox = new QMessageBox(this);
    confirmBox->setWindowTitle(QStringLiteral("确认完成"));
    confirmBox->setText(QStringLiteral("确定完成 %1号桌 的服务吗？").arg(tableNumber));
    confirmBox->setInformativeText(QStringLiteral("完成后该桌号将变为空闲状态。"));
    confirmBox->setIcon(QMessageBox::Question);
    confirmBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox->setDefaultButton(QMessageBox::Yes);

    // 设置按钮文本
    if (auto* yesBtn = confirmBox->button(QMessageBox::Yes)) {
        yesBtn->setText(QStringLiteral("确认"));
    }
    if (auto* noBtn = confirmBox->button(QMessageBox::No)) {
        noBtn->setText(QStringLiteral("取消"));
    }

    int result = confirmBox->exec();
    confirmBox->deleteLater();

    if (result == QMessageBox::Yes) {
        // 发射信号
        emit tableCompleted(tableNumber);

        // 将桌号设置为空闲（而不是移除）
        m_allTables[index].is_idle = true;
        m_allTables[index].customer_id = 0;
        m_allTables[index].order_id = 0;
        m_allTables[index].status = TableServiceStatus::Pending;
        m_allTables[index].has_call = false;
        m_allTables[index].dishes.clear();

        rebuildList(m_allTables);

        ElaMessageBar::success(ElaMessageBarType::BottomRight,
                              QStringLiteral("成功"),
                              QStringLiteral("服务已完成，%1号桌已空闲").arg(tableNumber),
                              2000, this);
    }

}

void ServiceRequest_Page::onHandleCall(int tableNumber)
{
    for (auto& table : m_allTables) {
        if (table.table_number == tableNumber) {
            table.has_call = false;
            rebuildList(m_allTables);

            emit callHandled(tableNumber);

            ElaMessageBar::success(ElaMessageBarType::BottomRight,
                                  QStringLiteral("成功"),
                                  QStringLiteral("已处理呼叫"),
                                  2000, this);
            return;
        }
    }
}

void ServiceRequest_Page::onDishServed(int tableNumber, const QString& dishName)
{
    // 更新本地数据中的菜品状态
    for (auto& table : m_allTables) {
        if (table.table_number == tableNumber) {
            for (auto& dish : table.dishes) {
                if (dish.dish.name == dishName && !dish.isServed) {
                    dish.isServed = true;
                    break;
                }
            }
            break;
        }
    }
}
