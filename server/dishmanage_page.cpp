#include "dishmanage_page.h"
#include "disheditdialog.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QAbstractButton>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonObject>
#include <QJsonArray>

#include "ElaLineEdit.h"
#include "ElaText.h"
#include "ElaPushButton.h"
#include "ElaScrollArea.h"
#include "ElaMessageBar.h"

#include "dao/DishDao.h"

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



}

void DishManage_Page::onSearchTextChanged(const QString& text)
{
    m_keyword = text.trimmed();
    m_searchDebounce->start();
}

void DishManage_Page::setDatabase(const QSqlDatabase& db)
{
    m_db = db;
}

void DishManage_Page::loadDishesFromDatabase()
{
    if (!m_db.isValid()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            QStringLiteral("数据库连接无效"), 2000, this);
        return;
    }

    if (!m_db.isOpen()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            QStringLiteral("数据库连接未打开"), 2000, this);
        return;
    }

    // 使用DishDao查询菜品
    db::DishDao dishDao(m_db);
    QJsonObject result = dishDao.listAll();

    int code = result.value("code").toInt();
    if (code != 200) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            result.value("msg").toString(), 2000, this);
        return;
    }

    QJsonArray dishArray = result.value("data").toArray();
    QList<Dish> dishes;

    for (const QJsonValue& val : dishArray) {
        QJsonObject obj = val.toObject();
        Dish dish;
        dish.dish_id = obj.value("dish_id").toInt();
        dish.name = obj.value("name").toString();
        dish.price = obj.value("price").toDouble();
        dish.category = obj.value("category").toString();
        dish.url = obj.value("url").toString();
        dish.description = obj.value("description").toString();
        dish.rating = obj.value("rating").toDouble();

        dishes.append(dish);
    }

    setDishList(dishes);
    ElaMessageBar::success(ElaMessageBarType::BottomRight, QStringLiteral("成功"),
                          QStringLiteral("加载了 %1 个菜品").arg(dishes.size()), 1500, this);
}

bool DishManage_Page::saveDishToDatabase(const Dish& dish)
{
    if (!m_db.isValid() || !m_db.isOpen()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            QStringLiteral("数据库连接未打开"), 2000, this);
        return false;
    }

    // 使用DishDao插入菜品
    db::DishDao dishDao(m_db);

    QJsonObject dishObj;
    dishObj.insert("name", dish.name);
    dishObj.insert("price", dish.price);
    dishObj.insert("category", dish.category);
    dishObj.insert("url", dish.url);
    dishObj.insert("description", dish.description);

    QJsonObject result = dishDao.insertDish(dishObj);

    int code = result.value("code").toInt();
    if (code != 200) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            QStringLiteral("添加菜品失败: ") + result.value("msg").toString(), 2000, this);
        return false;
    }

    return true;
}

bool DishManage_Page::updateDishInDatabase(const Dish& dish)
{
    if (!m_db.isValid() || !m_db.isOpen()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            QStringLiteral("数据库连接未打开"), 2000, this);
        return false;
    }

    // 使用DishDao更新菜品
    db::DishDao dishDao(m_db);

    QJsonObject dishObj;
    dishObj.insert("dish_id", dish.dish_id);
    dishObj.insert("name", dish.name);
    dishObj.insert("price", dish.price);
    dishObj.insert("category", dish.category);
    dishObj.insert("url", dish.url);
    dishObj.insert("description", dish.description);

    QJsonObject result = dishDao.updateDish(dishObj);

    int code = result.value("code").toInt();
    if (code != 200) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            QStringLiteral("更新菜品失败: ") + result.value("msg").toString(), 2000, this);
        return false;
    }

    return true;
}

bool DishManage_Page::deleteDishFromDatabase(int dishId)
{
    if (!m_db.isValid() || !m_db.isOpen()) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            QStringLiteral("数据库连接未打开"), 2000, this);
        return false;
    }

    // 使用DishDao删除菜品
    db::DishDao dishDao(m_db);
    QJsonObject result = dishDao.deleteDish(dishId);

    int code = result.value("code").toInt();
    if (code != 200) {
        ElaMessageBar::error(ElaMessageBarType::BottomRight, QStringLiteral("错误"),
                            QStringLiteral("删除菜品失败: ") + result.value("msg").toString(), 2000, this);
        return false;
    }

    return true;
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
    dialog->setAddMode(false);

    if (dialog->exec() == QDialog::Accepted) {
        Dish updatedDish = dialog->dish();

        // 更新到数据库
        if (updateDishInDatabase(updatedDish)) {
            // 更新本地数据
            *foundDish = updatedDish;

            // 刷新显示
            applyFilterNow();

            ElaMessageBar::success(ElaMessageBarType::BottomRight,
                                  QStringLiteral("成功"),
                                  QStringLiteral("菜品信息已更新"),
                                  2000, this);
        }
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
        // 从数据库删除
        if (deleteDishFromDatabase(dishId)) {
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

        // 保存到数据库
        if (saveDishToDatabase(dish)) {
            // 重新从数据库加载所有菜品（包括新生成的ID）
            loadDishesFromDatabase();

            ElaMessageBar::success(ElaMessageBarType::BottomRight,
                                  QStringLiteral("成功"),
                                  QStringLiteral("菜品添加成功"),
                                  2000, this);
        }
    }

    dialog->deleteLater();
}
