#pragma once
#include <QJsonObject>

class DishService {
public:
    static DishService* getInstance();

    QJsonObject list();

private:
    static DishService* dishService;

    explicit DishService();
};
