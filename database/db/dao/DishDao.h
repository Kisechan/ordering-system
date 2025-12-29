#pragma once
#include <QJsonObject>
#include <QSqlDatabase>

namespace db {

class DishDao {
public:
    explicit DishDao(QSqlDatabase db): db_(std::move(db)) {}

    QJsonObject listAll() const;
    QJsonObject findById(int dishId) const;

    QJsonObject insertDish(const QJsonObject& dish) const;
    QJsonObject updateDish(const QJsonObject& dish) const;
    QJsonObject deleteDish(int dishId) const;

private:
    QSqlDatabase db_;
};

} // namespace db
