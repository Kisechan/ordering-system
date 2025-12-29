#pragma once
#include <QJsonObject>
#include <QSqlDatabase>
#include <QVector>
#include "../entity/DishCount.h"

namespace db {

class OrderService {
public:
    explicit OrderService(QSqlDatabase db): db_(std::move(db)) {}

    QJsonObject submitOrder(int userId, const QVector<DishCount>& dishes);
    QJsonObject listOrdersByUser(int userId) const;
    QJsonObject submitComment(int orderId, const QString& comment);
    QJsonObject listAdminOrdersBrief() const;

private:
    QSqlDatabase db_;
};

} // namespace db
