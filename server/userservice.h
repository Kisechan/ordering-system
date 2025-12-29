#pragma once

#include <QJsonObject>

class UserService {
public:
    static UserService* getInstance();

    QJsonObject login(QString username, QString password);

    QJsonObject registerAccount(QString username, QString password);
private:
    static UserService* userService;

    explicit UserService();
};
