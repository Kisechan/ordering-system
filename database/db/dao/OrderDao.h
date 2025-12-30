#pragma once
#include <QJsonObject>
#include <QSqlDatabase>
#include <QString>

namespace db {

class OrderDao {
public:
    explicit OrderDao(QSqlDatabase db): db_(std::move(db)) {}

    QJsonObject insertOrder(int userId, double totalAmount, const QString& comment);
    QJsonObject updateComment(int orderId, const QString& comment);
    QJsonObject listByUser(int userId) const;
    QJsonObject listAll() const;

private:
    QSqlDatabase db_;
};

} // namespace db
