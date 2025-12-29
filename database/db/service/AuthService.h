#pragma once
#include <QJsonObject>
#include <QSqlDatabase>
#include <QString>

namespace db {

class AuthService {
public:
    explicit AuthService(QSqlDatabase db): db_(std::move(db)) {}
    QJsonObject login(const QString& username, const QString& password) const;
    QJsonObject registerUser(const QString& username, const QString& password) const;

private:
    QSqlDatabase db_;
};

} // namespace db
