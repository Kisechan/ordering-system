#pragma once
#include <QJsonObject>
#include <QSqlDatabase>

namespace db {

class OrderDishDao {
public:
    explicit OrderDishDao(QSqlDatabase db): db_(std::move(db)) {}

    QJsonObject insertOne(int orderId, int dishId);
    QJsonObject deleteByOrderId(int orderId);
    QJsonObject listDishIdsByOrderId(int orderId) const;

private:
    QSqlDatabase db_;
};

} // namespace db
