#pragma once

#include "ElaScrollPage.h"
#include <QList>
#include <QString>

#include "DishCard.h"

class ElaLineEdit;
class QVBoxLayout;
class QWidget;
class QTimer;

class HomePage : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit HomePage(QWidget* parent = nullptr);

    void setDishList(const QList<Dish>& dishes);

signals:
    void searchRequested(const QString& keyword);
    void addToCartRequested(const Dish& dish, int qty);

private slots:
    void onSearchTextChanged(const QString& text);
    void onSearchReturnPressed();
    void applyFilterNow();

private:
    void rebuildList(const QList<Dish>& dishes);

private:
    ElaLineEdit*    m_searchEdit = nullptr;
    QTimer*         m_searchDebounce = nullptr;

    QWidget*        m_listContainer = nullptr;
    QVBoxLayout*    m_listLayout = nullptr;

    QList<Dish>     m_allDishes;
    QString         m_keyword;
};
