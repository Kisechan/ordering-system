#pragma once
#include <QString>
#include <QList>

struct Dish {
    int dish_id = 0;
    QString name;
    double price = 0.0;
    QString category;
    double rating = 0.0;
    QString url;
    QString description;
};

struct CartItem {
    Dish dish;
    int qty = 1;
};

struct OrderDraft {
    QList<CartItem> items;
    int totalCount = 0;
    double totalPrice = 0.0;
};
