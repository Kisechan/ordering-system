#pragma once
#include <QJsonObject>
#include <QSqlDatabase>

namespace db {

class DishService {
public:
    explicit DishService(QSqlDatabase db): db_(std::move(db)) {}
    QJsonObject listAll() const;
    QJsonObject getById(int dishId) const;

private:
    QSqlDatabase db_;
};

} // namespace db
