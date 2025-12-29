#pragma once
namespace db {

class DishCount {
public:
    DishCount() = default;
    DishCount(int dishId, int count): dishId_(dishId), count_(count) {}

    int dishId() const { return dishId_; }
    void setDishId(int v) { dishId_ = v; }

    int count() const { return count_; }
    void setCount(int v) { count_ = v; }

private:
    int dishId_ = 0;
    int count_ = 0;
};

} // namespace db
