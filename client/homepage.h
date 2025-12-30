#pragma once

#include "ElaScrollPage.h"
#include <QList>
#include <QString>

#include "DishCard.h"
#include "carttype.h"

class ElaLineEdit;
class QVBoxLayout;
class QWidget;
class QTimer;
class NetworkManager;
class ElaText;

class HomePage : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit HomePage(NetworkManager* networkMgr = nullptr, QWidget* parent = nullptr);

    void setDishList(const QList<Dish>& dishes);

signals:
    void searchRequested(const QString& keyword);
    void addToCartRequested(const Dish& dish, int qty);

private slots:
    void onSearchTextChanged(const QString& text);
    void onSearchReturnPressed();
    void applyFilterNow();
    
    // NetworkManager 响应槽
    void onDishListReceived(const QJsonArray& dishes);
    void onDishListError(const QString& error);

private:
    void rebuildList(const QList<Dish>& dishes);
    void showLoadingState();
    void showErrorState(const QString& error);

private:
    ElaLineEdit*    m_searchEdit = nullptr;
    QTimer*         m_searchDebounce = nullptr;

    QWidget*        m_listContainer = nullptr;
    QVBoxLayout*    m_listLayout = nullptr;
    ElaText*        m_statusText = nullptr;  // 状态提示文本

    QList<Dish>     m_allDishes;
    QString         m_keyword;
    
    NetworkManager* m_networkMgr = nullptr;
};
