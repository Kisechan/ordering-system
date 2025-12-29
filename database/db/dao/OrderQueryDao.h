#pragma once
#include <QJsonObject>
#include <QSqlDatabase>

namespace db {

class OrderQueryDao {
public:
    explicit OrderQueryDao(QSqlDatabase db): db_(std::move(db)) {}

    QJsonObject getOrderHistoryByUser(int userId) const;
    QJsonObject getAdminOrderBriefs() const;

private:
    QSqlDatabase db_;
};

} // namespace db
