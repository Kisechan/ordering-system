#pragma once
#ifndef DISHMANAGE_PAGE_H
#define DISHMANAGE_PAGE_H

#include "ElaScrollPage.h"
#include <QList>
#include <QString>

#include "dishcard.h"

class ElaLineEdit;
class ElaPushButton;
class QVBoxLayout;
class QWidget;
class QTimer;

class DishManage_Page : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit DishManage_Page(QWidget* parent = nullptr);

    void setDishList(const QList<Dish>& dishes);

signals:
    void searchRequested(const QString& keyword); // 查询
    void dishUpdated(const Dish& dish);     // 菜品被修改
    void dishDeleted(int dishId);           // 菜品被删除
    void dishAdded(const Dish& dish);       // 新增菜品

private slots:
    void onSearchTextChanged(const QString& text);
    void onSearchReturnPressed();
    void applyFilterNow();
    void onEditDish(int dishId);
    void onDeleteDish(int dishId);
    void onAddDish();

private:
    void rebuildList(const QList<Dish>& dishes);

private:
    ElaLineEdit*    m_searchEdit = nullptr;
    ElaPushButton*  m_addBtn = nullptr;
    QTimer*         m_searchDebounce = nullptr;

    QWidget*        m_listContainer = nullptr;
    QVBoxLayout*    m_listLayout = nullptr;

    QList<Dish>     m_allDishes;
    QString         m_keyword;
};

#endif // DISHMANAGE_PAGE_H
