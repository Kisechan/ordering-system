#include "dishmanage_page.h"
#include "disheditdialog.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QAbstractButton>
#include <QMessageBox>
#include <QDebug>

#include "ElaLineEdit.h"
#include "ElaText.h"
#include "ElaPushButton.h"
#include "ElaScrollArea.h"
#include "ElaMessageBar.h"

DishManage_Page::DishManage_Page(QWidget* parent):
    ElaScrollPage(parent)
{
    setTitleVisible(false);
    setPageTitleSpacing(0);

    // ===== 创建主容器（不滚动） =====
    auto* mainContainer = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(20, 18, 20, 0);
    mainLayout->setSpacing(12);

    // ===== 顶部操作栏（搜索框 + 增加按钮） =====
    auto* topBar = new QWidget(mainContainer);
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(10);

    m_searchEdit = new ElaLineEdit(topBar);
    m_searchEdit->setPlaceholderText(QStringLiteral("搜索菜品名称 / 品类 / 描述"));
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumHeight(40);
    topLayout->addWidget(m_searchEdit, 1);

    m_addBtn = new ElaPushButton(QStringLiteral("增加菜品"), topBar);
    m_addBtn->setFixedSize(120, 40);
    QFont btnFont = m_addBtn->font();
    btnFont.setPixelSize(16);
    m_addBtn->setFont(btnFont);
    m_addBtn->setStyleSheet(
        "ElaPushButton { "
        "padding: 8px 16px; "
        "background-color: #4CAF50;"
        "}"
    );
    topLayout->addWidget(m_addBtn);

    mainLayout->addWidget(topBar);

    // ===== 滚动内容区域（仅卡片列表滚动）=====
    auto* scrollContent = new QWidget(mainContainer);
    auto* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 0, 0, 18);
    scrollLayout->setSpacing(12);

    // ===== 卡片列表容器（一行一个卡片）=====
    m_listContainer = new QWidget(scrollContent);
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(12);
    scrollLayout->addWidget(m_listContainer);

    // 让内容从顶部开始排（空白永远在下面）
    scrollLayout->addStretch(1);

    mainLayout->addWidget(scrollContent, 1);

    // 把 mainContainer 挂到 ElaScrollPage 内部滚动体系
    // 注意：现在只有 scrollContent 会滚动，搜索框固定在顶部
    addCentralWidget(mainContainer, /*isWidgetResizeable=*/true, /*isVerticalGrabGesture=*/true);

    // 内容少时也贴顶（防止被居中）
    if (auto* sa = this->findChild<ElaScrollArea*>()) {
        sa->setAlignment(Qt::AlignTop);
    }

    // ===== 搜索：防抖过滤 + 回车给后端 =====
    m_searchDebounce = new QTimer(this);
    m_searchDebounce->setSingleShot(true);
    m_searchDebounce->setInterval(200);

    connect(m_searchEdit, &ElaLineEdit::textChanged,this, &DishManage_Page::onSearchTextChanged);
    connect(m_searchDebounce, &QTimer::timeout,this, &DishManage_Page::applyFilterNow);
    connect(m_searchEdit, &ElaLineEdit::returnPressed,this, &DishManage_Page::onSearchReturnPressed);
    connect(m_addBtn, &ElaPushButton::clicked, this, &DishManage_Page::onAddDish);

    // ===== 演示：先放 1 个宫保鸡丁卡片 =====
    Dish d;
    d.dish_id = 1;
    d.name = QStringLiteral("宫保鸡丁");
    d.price = 28.00;
    d.category = QStringLiteral("川菜");
    d.rating = 4.8;
    d.url = QStringLiteral(":/Image/vvan.jpg");
    d.description = QStringLiteral("经典川菜，微辣香脆");

    Dish d1;
    d1.dish_id = 2;
    d1.name = QStringLiteral("好好好");
    d1.price = 88.00;
    d1.category = QStringLiteral("湘菜");
    d1.rating = 4.9;
    d1.url = QStringLiteral(":/include/Image/Moon.jpg");
    d1.description = QStringLiteral("good");
    // 真实情况每个dish不同
    setDishList({d,d1,d,d,d1,d,d,d,d1,d,d,d,d1,d});
}

void DishManage_Page::onSearchTextChanged(const QString& text)
{
    m_keyword = text.trimmed();
    m_searchDebounce->start();
}

void DishManage_Page::setDishList(const QList<Dish>& dishes)
{
    m_allDishes = dishes;
    applyFilterNow();
}

void DishManage_Page::onSearchReturnPressed()
{
    emit searchRequested(m_keyword); // 后端接口预留：回车再请求服务器
}

void DishManage_Page::applyFilterNow()
{
    QList<Dish> show;
    if (m_keyword.isEmpty()) {
        show = m_allDishes;
    } else {
        for (const auto& d : m_allDishes) {
            if (d.name.contains(m_keyword, Qt::CaseInsensitive) ||
                d.category.contains(m_keyword, Qt::CaseInsensitive) ||
                d.description.contains(m_keyword, Qt::CaseInsensitive)) {
                show.push_back(d);
            }
        }
    }
    rebuildList(show);
}

void DishManage_Page::rebuildList(const QList<Dish>& dishes)
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

    if (dishes.isEmpty()) {
        // 空态也放在列表里，但它在搜索框下面、不会重叠
        auto* empty = new ElaText(QStringLiteral("没有找到相关菜品"), 14, m_listContainer);
        empty->setStyleSheet("color:#888888;");
        empty->setAlignment(Qt::AlignHCenter);
        m_listLayout->addWidget(empty);
        return;
    }

    for (const auto& d : dishes) {
        auto* card = new DishCard(m_listContainer);
        card->setDish(d);

        connect(card, &DishCard::editRequested, this, &DishManage_Page::onEditDish);
        connect(card, &DishCard::deleteRequested, this, &DishManage_Page::onDeleteDish);

        m_listLayout->addWidget(card);
    }
}

void DishManage_Page::onEditDish(int dishId)
{
    // 从列表中查找对应的菜品
    Dish* foundDish = nullptr;
    for (auto& dish : m_allDishes) {
        if (dish.dish_id == dishId) {
            foundDish = &dish;
            break;
        }
    }

    if (!foundDish) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight,
                              QStringLiteral("提示"),
                              QStringLiteral("菜品未找到"),
                              2000, this);
        return;
    }

    // 显示编辑对话框
    auto* dialog = new DishEditDialog(this);
    dialog->setDish(*foundDish);

    if (dialog->exec() == QDialog::Accepted) {
        Dish updatedDish = dialog->dish();

        // 更新本地数据
        *foundDish = updatedDish;

        // 发射信号通知外部（用于数据库更新）
        emit dishUpdated(updatedDish);

        // 刷新显示
        applyFilterNow();

        ElaMessageBar::success(ElaMessageBarType::BottomRight,
                              QStringLiteral("成功"),
                              QStringLiteral("菜品信息已更新"),
                              2000, this);
    }

    dialog->deleteLater();
}

void DishManage_Page::onDeleteDish(int dishId)
{
    // 从列表中查找对应的菜品
    int index = -1;
    QString dishName;
    for (int i = 0; i < m_allDishes.size(); ++i) {
        if (m_allDishes[i].dish_id == dishId) {
            index = i;
            dishName = m_allDishes[i].name;
            break;
        }
    }

    if (index == -1) {
        ElaMessageBar::warning(ElaMessageBarType::BottomRight,
                              QStringLiteral("提示"),
                              QStringLiteral("菜品未找到"),
                              2000, this);
        return;
    }

    // 显示确认对话框
    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle(QStringLiteral("确认删除"));
    confirmBox.setText(QStringLiteral("确定要删除菜品 \"%1\" 吗？").arg(dishName));
    confirmBox.setInformativeText(QStringLiteral("此操作不可撤销，该菜品将从数据库中永久删除。"));
    confirmBox.setIcon(QMessageBox::Warning);
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);
    confirmBox.button(QMessageBox::Yes)->setText(QStringLiteral("确认"));
    confirmBox.button(QMessageBox::No)->setText(QStringLiteral("取消"));

    if (confirmBox.exec() == QMessageBox::Yes) {
        // 发射信号通知外部删除数据库记录
        emit dishDeleted(dishId);

        // 从本地列表中移除
        m_allDishes.removeAt(index);

        // 刷新显示
        applyFilterNow();

        ElaMessageBar::success(ElaMessageBarType::BottomRight,
                              QStringLiteral("成功"),
                              QStringLiteral("菜品已删除"),
                              2000, this);
    }
}

void DishManage_Page::onAddDish()
{
    // 创建新菜品对话框
    auto* dialog = new DishEditDialog(this);
    dialog->setWindowTitle(QStringLiteral("新增菜品"));

    // 设置一个空的菜品（新增模式）
    Dish newDish;
    newDish.dish_id = 0;  // 0表示新增，数据库会自动生成ID
    newDish.rating = 0.0; // 默认未评分
    newDish.name = "";
    newDish.price = 0.0;
    newDish.category = "";
    newDish.url = "";
    newDish.description = "";

    dialog->setDish(newDish);
    dialog->setAddMode(true);  // 设置为新增模式

    if (dialog->exec() == QDialog::Accepted) {
        Dish dish = dialog->dish();

        // ===== 字段验证 =====

        // 验证必填字段
        if (dish.name.trimmed().isEmpty()) {
            ElaMessageBar::warning(ElaMessageBarType::BottomRight,
                                  QStringLiteral("验证失败"),
                                  QStringLiteral("菜品名称不能为空"),
                                  2000, this);
            dialog->deleteLater();
            return;
        }

        if (dish.price <= 0) {
            ElaMessageBar::warning(ElaMessageBarType::BottomRight,
                                  QStringLiteral("验证失败"),
                                  QStringLiteral("价格必须大于0"),
                                  2000, this);
            dialog->deleteLater();
            return;
        }

        if (dish.category.trimmed().isEmpty()) {
            ElaMessageBar::warning(ElaMessageBarType::BottomRight,
                                  QStringLiteral("验证失败"),
                                  QStringLiteral("品类不能为空"),
                                  2000, this);
            dialog->deleteLater();
            return;
        }

        // 处理描述字段：如果为空则设置为空字符串（不是null）
        if (dish.description.isNull()) {
            dish.description = "";
        }

        // 处理图片URL：如果为空则使用默认图片
        if (dish.url.trimmed().isEmpty()) {
            dish.url = ":/Image/vvan.jpg";  // 默认图片路径
        }

        // 评分默认为0（未评分）
        dish.rating = 0.0;

        // 生成临时ID（实际应由数据库生成并返回）
        // 这里使用当前列表最大ID+1作为临时ID
        int maxId = 0;
        for (const auto& d : m_allDishes) {
            if (d.dish_id > maxId) {
                maxId = d.dish_id;
            }
        }
        dish.dish_id = maxId + 1;

        // ===== 添加到本地列表 =====
        m_allDishes.append(dish);

        // ===== 发射信号通知外部添加到数据库 =====
        emit dishAdded(dish);

        // ===== 刷新显示 =====
        applyFilterNow();

        // ===== 显示成功提示 =====
        ElaMessageBar::success(ElaMessageBarType::BottomRight,
                              QStringLiteral("成功"),
                              QStringLiteral("菜品添加成功"),
                              2000, this);
    }

    dialog->deleteLater();
}
