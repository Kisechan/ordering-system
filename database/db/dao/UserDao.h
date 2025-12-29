#pragma once
#include <QJsonObject>
#include <QSqlDatabase>
#include <QString>

namespace db {

class UserDao {
public:
    explicit UserDao(QSqlDatabase db): db_(std::move(db)) {}

    QJsonObject findByUsername(const QString& username) const;
    QJsonObject findById(int userId) const;
    QJsonObject insertUser(const QString& username, const QString& password) const;

private:
    QSqlDatabase db_;
};

} // namespace db
