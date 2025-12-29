#include "HomePage.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>

#include "ElaLineEdit.h"
#include "ElaText.h"
#include "ElaScrollArea.h"

HomePage::HomePage(QWidget* parent)
    : ElaScrollPage(parent)
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

    // ===== 演示：先放 1 个宫保鸡丁卡片 =====
    Dish d;
    d.dish_id = 1;
    d.name = QStringLiteral("宫保鸡丁");
    d.price = 28.00;
    d.category = QStringLiteral("川菜");
    d.rating = 4.8;
    d.url = QStringLiteral(":/Image/vvan.jpg"); // 先用资源占位
    d.description = QStringLiteral("经典川菜，微辣香脆");
    setDishList({d,d,d,d,d,d,d,d,d,d,d,d,d,d});
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
        auto* empty = new ElaText(QStringLiteral("没有找到相关菜品"), 14, m_listContainer);
        empty->setStyleSheet("color:#888888;");
        empty->setAlignment(Qt::AlignHCenter);
        m_listLayout->addWidget(empty);
        return;
    }

    for (const auto& d : dishes) {
        auto* card = new DishCard(m_listContainer);
        card->setDish(d);

        connect(card, &DishCard::addToCartRequested, this,
                [](int dishId, int qty) {
                    // TODO: 后端购物车接口
                    qDebug("addToCartRequested dishId=%d qty=%d", dishId, qty);
                });

        m_listLayout->addWidget(card);
    }
}
