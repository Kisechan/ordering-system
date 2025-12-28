#pragma once
#include <QSqlDatabase>
#include <QVector>
#include "ApiResponse.h"
#include "../entity/Dish.h"

namespace db {

class DishDao {
public:
    explicit DishDao(QSqlDatabase db): db_(std::move(db)) {}

    // 获取菜品详细信息
    ApiResponse<Dish> getById(int dishId) const;

    // 菜品列表
    ApiResponse<QVector<Dish>> listAll() const;

    // 新增菜品
    ApiResponseVoid addDish(const Dish& dish) const;

    // 修改菜品
    ApiResponseVoid updateDish(const Dish& dish) const;

    // 删除菜品
    ApiResponseVoid deleteDish(int dishId) const;

private:
    QSqlDatabase db_;
};

} // namespace db
