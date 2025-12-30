#include "HomePage.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

#include "ElaLineEdit.h"
#include "ElaText.h"
#include "ElaScrollArea.h"
#include "ElaMessageBar.h"
#include "NetworkManager.h"

HomePage::HomePage(NetworkManager* networkMgr, QWidget* parent)
    : ElaScrollPage(parent), m_networkMgr(networkMgr)
{
    setTitleVisible(false);
    setPageTitleSpacing(0);

    // ===== 滚动内容根容器（整个页面就一个 scrollball）=====
    auto* content = new QWidget(this);
    auto* root = new QVBoxLayout(content);
    root->setContentsMargins(20, 18, 20, 18);
    root->setSpacing(12);

    // ===== 顶部搜索框（在滚动内容里，跟着一起滚动）=====
    m_searchEdit = new ElaLineEdit(content);
    m_searchEdit->setPlaceholderText(QStringLiteral("搜索菜品名称 / 品类 / 描述"));
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumHeight(40);
    root->addWidget(m_searchEdit);

    // ===== 卡片列表容器（一行一个卡片）=====
    m_listContainer = new QWidget(content);
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(12);
    root->addWidget(m_listContainer);

    // 让内容从顶部开始排（空白永远在下面）
    root->addStretch(1);

    // 把 content 挂到 ElaScrollPage 内部滚动体系
    addCentralWidget(content, /*isWidgetResizeable=*/true, /*isVerticalGrabGesture=*/true);

    // 内容少时也贴顶（防止被居中）
    if (auto* sa = this->findChild<ElaScrollArea*>()) {
        sa->setAlignment(Qt::AlignTop);
    }

    // ===== 搜索：防抖过滤 + 回车给后端 =====
    m_searchDebounce = new QTimer(this);
    m_searchDebounce->setSingleShot(true);
    m_searchDebounce->setInterval(200);

    connect(m_searchEdit, &ElaLineEdit::textChanged,
            this, &HomePage::onSearchTextChanged);
    connect(m_searchDebounce, &QTimer::timeout,
            this, &HomePage::applyFilterNow);
    connect(m_searchEdit, &ElaLineEdit::returnPressed,
            this, &HomePage::onSearchReturnPressed);

    // ===== NetworkManager 集成：连接信号 =====
    if (m_networkMgr) {
        connect(m_networkMgr, &NetworkManager::dishListReceived,
                this, &HomePage::onDishListReceived);
        connect(m_networkMgr, &NetworkManager::dishListError,
                this, &HomePage::onDishListError);
        
        // 初始化时请求菜品列表
        showLoadingState();
        m_networkMgr->getDishList();
    } else {
        // NetworkManager 为空时显示演示数据
        showLoadingState();
        QTimer::singleShot(1000, this, [this]() {
            Dish d;
            d.dish_id = 1;
            d.name = QStringLiteral("宫保鸡丁");
            d.price = 28.00;
            d.category = QStringLiteral("川菜");
            d.rating = 4.8;
            d.url = QStringLiteral(":/Image/vvan.jpg");
            d.description = QStringLiteral("经典川菜，微辣香脆");
            setDishList({d, d, d, d, d, d, d, d, d, d, d, d, d, d});
        });
    }
}

void HomePage::setDishList(const QList<Dish>& dishes)
{
    m_allDishes = dishes;
    applyFilterNow();
}

void HomePage::onSearchTextChanged(const QString& text)
{
    m_keyword = text.trimmed();
    m_searchDebounce->start();
}

void HomePage::onSearchReturnPressed()
{
    emit searchRequested(m_keyword); // 后端接口预留：回车再请求服务器
}

void HomePage::applyFilterNow()
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

void HomePage::rebuildList(const QList<Dish>& dishes)
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
        m_statusText = new ElaText(QStringLiteral("没有找到相关菜品"), 14, m_listContainer);
        m_statusText->setStyleSheet("color:#888888;");
        m_statusText->setAlignment(Qt::AlignHCenter);
        m_listLayout->addWidget(m_statusText);
        return;
    }

    for (const auto& d : dishes) {
        auto* card = new DishCard(m_listContainer);
        card->setDish(d);

        connect(card, &DishCard::addToCartRequested, this,
                [this, d](int /*dishId*/, int qty) {
                    qDebug() << "[HomePage] 添加菜品到购物车:"
                             << "name=" << d.name 
                             << ", dish_id=" << d.dish_id 
                             << ", qty=" << qty 
                             << ", price=" << d.price;
                    
                    // 显示添加成功提示
                    QString msg = QStringLiteral("已添加 %1 × %2 到购物车")
                                    .arg(d.name)
                                    .arg(qty);
                    ElaMessageBar::success(ElaMessageBarType::BottomRight, 
                                          QStringLiteral("成功"),
                                          msg, 
                                          2000, this);
                    
                    emit addToCartRequested(d, qty);
                });

        m_listLayout->addWidget(card);
    }
}

void HomePage::showLoadingState()
{
    // 清空列表
    while (m_listLayout && m_listLayout->count() > 0) {
        QLayoutItem* it = m_listLayout->takeAt(0);
        if (!it) break;
        if (QWidget* w = it->widget()) {
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete it;
    }
    
    // 显示加载状态
    m_statusText = new ElaText(QStringLiteral("加载中..."), 14, m_listContainer);
    m_statusText->setStyleSheet("color:#999999;");
    m_statusText->setAlignment(Qt::AlignHCenter);
    if (m_listLayout) {
        m_listLayout->addWidget(m_statusText);
    }
}

void HomePage::showErrorState(const QString& error)
{
    // 清空列表
    while (m_listLayout && m_listLayout->count() > 0) {
        QLayoutItem* it = m_listLayout->takeAt(0);
        if (!it) break;
        if (QWidget* w = it->widget()) {
            w->setParent(nullptr);
            w->deleteLater();
        }
        delete it;
    }
    
    // 显示错误状态
    QString errMsg = QStringLiteral("加载菜品列表失败: ") + error;
    m_statusText = new ElaText(errMsg, 14, m_listContainer);
    m_statusText->setStyleSheet("color:#ff6b6b;");
    m_statusText->setAlignment(Qt::AlignHCenter);
    if (m_listLayout) {
        m_listLayout->addWidget(m_statusText);
    }
}

void HomePage::onDishListReceived(const QJsonArray& dishes)
{
    qDebug() << "[HomePage] 收到菜品列表，开始解析... 菜品数量:" << dishes.size();
    
    // 解析 QJsonArray 构造 Dish 对象列表
    QList<Dish> dishList;
    
    for (const QJsonValue& value : dishes) {
        QJsonObject dishObj = value.toObject();
        
        Dish d;
        d.dish_id = dishObj["dish_id"].toInt();
        d.name = dishObj["name"].toString();
        d.price = dishObj["price"].toDouble();
        d.category = dishObj["category"].toString();
        d.rating = dishObj["rating"].toDouble();
        d.url = dishObj["url"].toString();
        d.description = dishObj["description"].toString();
        
        qDebug() << "[HomePage]   菜品:" << d.name << ", ID:" << d.dish_id << ", 价格:" << d.price;
        dishList.append(d);
    }
    
    qDebug() << "[HomePage] 菜品解析完成，总计" << dishList.size() << "个";
    // 更新 UI 显示所有菜品
    setDishList(dishList);
}

void HomePage::onDishListError(const QString& error)
{
    qWarning() << "[HomePage] 菜品列表获取失败:" << error;
    showErrorState(error);
}
