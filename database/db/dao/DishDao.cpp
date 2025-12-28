#include "DishDao.h"
#include "DaoCommon.h"

namespace db {

ApiResponse<Dish> DishDao::getById(int dishId) const {
    ApiResponse<Dish> resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    QSqlQuery q(db_);
    q.prepare("SELECT dish_id, name, price, category, rating, url, description "
              "FROM t_dish WHERE dish_id=? LIMIT 1");
    q.addBindValue(dishId);

    if (!q.exec()) { resp.code = 500; resp.msg = sqlErr(q); return resp; }
    if (!q.next()) { resp.code = 404; resp.msg = "菜品不存在"; return resp; }

    Dish d;
    d.setDishId(q.value("dish_id").toInt());
    d.setName(q.value("name").toString());
    d.setPrice(q.value("price").toDouble());
    d.setCategory(q.value("category").toString());
    d.setRating(q.value("rating").toDouble());
    d.setUrl(q.value("url").toString());
    d.setDescription(q.value("description").toString());

    resp.code = 200;
    resp.msg = "获取成功";
    resp.data = d;
    return resp;
}

ApiResponse<QVector<Dish>> DishDao::listAll() const {
    ApiResponse<QVector<Dish>> resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    QSqlQuery q(db_);
    if (!q.exec("SELECT dish_id, name, price, category, rating, url, description "
                "FROM t_dish ORDER BY dish_id ASC")) {
        resp.code = 500;
        resp.msg = sqlErr(q);
        return resp;
    }

    QVector<Dish> out;
    while (q.next()) {
        Dish d;
        d.setDishId(q.value("dish_id").toInt());
        d.setName(q.value("name").toString());
        d.setPrice(q.value("price").toDouble());
        d.setCategory(q.value("category").toString());
        d.setRating(q.value("rating").toDouble());
        d.setUrl(q.value("url").toString());
        d.setDescription(q.value("description").toString());
        out.push_back(d);
    }

    resp.code = 200;
    resp.msg = "获取成功";
    resp.data = out;
    return resp;
}

ApiResponseVoid DishDao::addDish(const Dish& dish) const {
    ApiResponseVoid resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    QSqlQuery q(db_);
    q.prepare("INSERT INTO t_dish(name, price, category, rating, url, description) "
              "VALUES(?, ?, ?, ?, ?, ?)");
    q.addBindValue(dish.name());
    q.addBindValue(dish.price());
    q.addBindValue(dish.category());
    q.addBindValue(dish.rating());
    q.addBindValue(dish.url());
    q.addBindValue(dish.description());

    if (!q.exec()) { resp.code = 500; resp.msg = sqlErr(q); return resp; }

    resp.code = 200;
    resp.msg = "新增成功";
    return resp;
}

ApiResponseVoid DishDao::updateDish(const Dish& dish) const {
    ApiResponseVoid resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    QSqlQuery q(db_);
    q.prepare("UPDATE t_dish SET name=?, price=?, category=?, rating=?, url=?, description=? "
              "WHERE dish_id=?");
    q.addBindValue(dish.name());
    q.addBindValue(dish.price());
    q.addBindValue(dish.category());
    q.addBindValue(dish.rating());
    q.addBindValue(dish.url());
    q.addBindValue(dish.description());
    q.addBindValue(dish.dishId());

    if (!q.exec()) { resp.code = 500; resp.msg = sqlErr(q); return resp; }
    if (q.numRowsAffected() <= 0) { resp.code = 404; resp.msg = "菜品不存在"; return resp; }

    resp.code = 200;
    resp.msg = "修改成功";
    return resp;
}

ApiResponseVoid DishDao::deleteDish(int dishId) const {
    ApiResponseVoid resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    QSqlQuery q(db_);
    q.prepare("DELETE FROM t_dish WHERE dish_id=?");
    q.addBindValue(dishId);

    if (!q.exec()) { resp.code = 500; resp.msg = sqlErr(q); return resp; }
    if (q.numRowsAffected() <= 0) { resp.code = 404; resp.msg = "菜品不存在"; return resp; }

    resp.code = 200;
    resp.msg = "删除成功";
    return resp;
}

} // namespace db
