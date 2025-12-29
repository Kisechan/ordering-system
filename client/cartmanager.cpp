#include "cartmanager.h"

#include <QtMath>

CartManager::CartManager(QObject* parent)
    : QObject(parent)
{
    recalcTotalsAndEmitIfNeeded(true);
}

QList<CartItem> CartManager::items() const
{
    // 直接返回 values()，展示顺序不保证稳定；改成 QMap 或在这里 sort 可保证顺序稳定。
    return m_items.values();
}

bool CartManager::contains(int dishId) const
{
    return m_items.contains(dishId);
}

int CartManager::quantity(int dishId) const
{
    auto it = m_items.constFind(dishId);
    if (it == m_items.constEnd()) return 0;
    return it->qty;
}

void CartManager::clear()
{
    if (m_items.isEmpty()) return;
    m_items.clear();
    recalcTotalsAndEmitIfNeeded(true);
    emit cartChanged();
}

void CartManager::removeDish(int dishId)
{
    if (!m_items.contains(dishId)) return;
    m_items.remove(dishId);
    recalcTotalsAndEmitIfNeeded();
    emit itemChanged(dishId);
    emit cartChanged();
}

void CartManager::addDish(const Dish& dish, int addQty)
{
    if (addQty <= 0) return;

    auto it = m_items.find(dish.dish_id);
    if (it == m_items.end()) {
        CartItem ci;
        ci.dish = dish;     // 这里仅仅只存一个快照
        ci.qty = addQty;
        m_items.insert(dish.dish_id, ci);
    } else {
        it->qty += addQty;
        if (it->qty < 1) it->qty = 1;
        it->dish = dish;   // 后端信息可能变化，更新快照
    }

    recalcTotalsAndEmitIfNeeded();
    emit itemChanged(dish.dish_id);
    emit cartChanged();
}

void CartManager::setQuantity(int dishId, int qty)
{
    if (!m_items.contains(dishId)) return;

    if (qty <= 0) {
        removeDish(dishId);
        return;
    }

    auto it = m_items.find(dishId);
    if (it->qty == qty) return;

    it->qty = qty;
    recalcTotalsAndEmitIfNeeded();
    emit itemChanged(dishId);
    emit cartChanged();
}

OrderDraft CartManager::buildOrderDraft() const
{
    OrderDraft d;
    d.items = items();
    d.totalCount = totalCount();
    d.totalPrice = totalPrice();
    return d;
}

void CartManager::recalcTotalsAndEmitIfNeeded(bool forceEmit)
{
    int totalCnt = 0;
    double totalPrice = 0.0;

    for (auto it = m_items.constBegin(); it != m_items.constEnd(); ++it) {
        totalCnt += it->qty;
        totalPrice += it->dish.price * double(it->qty);
    }

    // 金额保留两位（避免浮点显示抖动）
    totalPrice = qRound64(totalPrice * 100.0) / 100.0;

    const bool changed =
        forceEmit ||
        (totalCnt != m_cachedTotalCount) ||
        (!qFuzzyCompare(totalPrice + 1.0, m_cachedTotalPrice + 1.0));

    m_cachedTotalCount = totalCnt;
    m_cachedTotalPrice = totalPrice;

    if (changed) {
        emit totalsChanged(m_cachedTotalCount, m_cachedTotalPrice);
    }
}
