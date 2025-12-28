#pragma once
#include <QJsonObject>

class OrderService {
public:
    static OrderService* getInstance();

    QJsonObject order(int userId, QList<int> dishIdList, QList<int> countList);

    QJsonObject listByUserId(int userId);

    QJsonObject comment(int userId, int orderId, QString comment);
private:
    static OrderService* orderService;

    explicit OrderService();
};
