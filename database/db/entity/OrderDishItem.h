#pragma once
#include <QString>

namespace db {

// In order history: (dish_id, name, price, count)
class OrderDishItem {
public:
    int dishId() const { return dishId_; }
    void setDishId(int v) { dishId_ = v; }

    const QString& name() const { return name_; }
    void setName(const QString& v) { name_ = v; }

    double price() const { return price_; }
    void setPrice(double v) { price_ = v; }

    int count() const { return count_; }
    void setCount(int v) { count_ = v; }

private:
    int dishId_ = 0;
    QString name_;
    double price_ = 0.0;
    int count_ = 0;
};

} // namespace db
