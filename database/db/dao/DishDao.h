#pragma once
#include <QSqlDatabase>
#include <QVector>
#include "ApiResponse.h"
#include "../entity/Dish.h"

namespace db {

class DishDao {
public:
    explicit DishDao(QSqlDatabase db): db_(std::move(db)) {}

    // design.md 4.1 获取菜品详细信息 / 7.1 查询菜品
    ApiResponse<Dish> getById(int dishId) const;

    // 菜品列表
    ApiResponse<QVector<Dish>> listAll() const;

    // design.md 7.2 新增菜品
    ApiResponseVoid addDish(const Dish& dish) const;

    // design.md 7.3 修改菜品
    ApiResponseVoid updateDish(const Dish& dish) const;

    // design.md 7.4 删除菜品
    ApiResponseVoid deleteDish(int dishId) const;

private:
    QSqlDatabase db_;
};

} // namespace db
