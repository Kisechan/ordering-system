#include "DishDao.h"
#include "DaoCommon.h"
#include "JsonUtil.h"
#include <QJsonArray>

namespace db {

static QJsonObject rowToDish(const QSqlQuery& q) {
    QJsonObject d;
    d.insert("dish_id", q.value("dish_id").toInt());
    d.insert("name", q.value("name").toString());
    d.insert("price", q.value("price").toDouble());
    d.insert("category", q.value("category").toString());
    d.insert("rating", q.value("rating").toDouble());
    d.insert("url", q.value("url").toString());
    d.insert("description", q.value("description").toString());
    return d;
}

QJsonObject DishDao::listAll() const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    if (!q.exec("SELECT dish_id, name, price, category, rating, url, description FROM t_dish ORDER BY dish_id ASC")) {
        return makeErr(500, sqlErr(q));
    }

    QJsonArray arr;
    while (q.next()) arr.append(rowToDish(q));

    return makeOk(arr);
}

QJsonObject DishDao::findById(int dishId) const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("SELECT dish_id, name, price, category, rating, url, description FROM t_dish WHERE dish_id=? LIMIT 1");
    q.addBindValue(dishId);

    if (!q.exec()) return makeErr(500, sqlErr(q));
    if (!q.next()) return makeOk(QJsonValue::Null);

    return makeOk(rowToDish(q));
}

QJsonObject DishDao::insertDish(const QJsonObject& dish) const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("INSERT INTO t_dish(name, price, category, rating, url, description) VALUES(?, ?, ?, ?, ?, ?)");
    q.addBindValue(dish.value("name").toString());
    q.addBindValue(dish.value("price").toDouble());
    q.addBindValue(dish.value("category").toString());
    q.addBindValue(dish.value("rating").toDouble());
    q.addBindValue(dish.value("url").toString());
    q.addBindValue(dish.value("description").toString());

    if (!q.exec()) return makeErr(500, sqlErr(q));
    return makeOk(QJsonValue::Null);
}

QJsonObject DishDao::updateDish(const QJsonObject& dish) const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("UPDATE t_dish SET name=?, price=?, category=?, rating=?, url=?, description=? WHERE dish_id=?");
    q.addBindValue(dish.value("name").toString());
    q.addBindValue(dish.value("price").toDouble());
    q.addBindValue(dish.value("category").toString());
    q.addBindValue(dish.value("rating").toDouble());
    q.addBindValue(dish.value("url").toString());
    q.addBindValue(dish.value("description").toString());
    q.addBindValue(dish.value("dish_id").toInt());

    if (!q.exec()) return makeErr(500, sqlErr(q));
    return makeOk(QJsonValue::Null);
}

QJsonObject DishDao::deleteDish(int dishId) const {
    QString err;
    if (!dbIsOpen(db_, &err)) return makeErr(500, err);

    QSqlQuery q(db_);
    q.prepare("DELETE FROM t_dish WHERE dish_id=?");
    q.addBindValue(dishId);

    if (!q.exec()) return makeErr(500, sqlErr(q));
    return makeOk(QJsonValue::Null);
}

} // namespace db
