#pragma once
#include <QJsonObject>
#include <QSqlDatabase>

namespace db {

class OrderDishDao {
public:
    explicit OrderDishDao(QSqlDatabase db): db_(std::move(db)) {}

    QJsonObject insertOne(int orderId, int dishId, int count);
    QJsonObject deleteByOrderId(int orderId);
    QJsonObject listDishIdsByOrderId(int orderId) const;
    QJsonObject updateRating(int orderId, int dishId, int rating);

private:
    QSqlDatabase db_;
};

} // namespace db
