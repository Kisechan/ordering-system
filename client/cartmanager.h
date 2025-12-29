#pragma once
#include <QObject>
#include <QHash>
#include <QList>

#include "carttype.h"

class CartManager : public QObject
{
    Q_OBJECT
public:
    explicit CartManager(QObject* parent = nullptr);

    // ===== 查询 =====
    bool isEmpty() const { return m_items.isEmpty(); }
    QList<CartItem> items() const;           // 给 UI 列表用
    bool contains(int dishId) const;
    int quantity(int dishId) const;

    int totalCount() const { return m_cachedTotalCount; }
    double totalPrice() const { return m_cachedTotalPrice; }

    OrderDraft buildOrderDraft() const;

    // ===== 修改 =====
    void clear();
    void removeDish(int dishId);

    // add：存在则累加；否则插入
    void addDish(const Dish& dish, int addQty = 1);

    // set：qty<=0 -> remove
    void setQuantity(int dishId, int qty);

signals:
    // 通知 UI 刷新
    void cartChanged();
    void itemChanged(int dishId);
    void totalsChanged(int totalCount, double totalPrice);

private:
    void recalcTotalsAndEmitIfNeeded(bool forceEmit = false);

private:
    QHash<int, CartItem> m_items; // key = dish_id

    int    m_cachedTotalCount = 0;
    double m_cachedTotalPrice = 0.0;
};
