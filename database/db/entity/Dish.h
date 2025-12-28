#pragma once
#include <QString>

namespace db {

class Dish {
public:
    int dishId() const { return dishId_; }
    void setDishId(int v) { dishId_ = v; }

    const QString& name() const { return name_; }
    void setName(const QString& v) { name_ = v; }

    double price() const { return price_; }
    void setPrice(double v) { price_ = v; }

    const QString& category() const { return category_; }
    void setCategory(const QString& v) { category_ = v; }

    double rating() const { return rating_; }
    void setRating(double v) { rating_ = v; }

    const QString& url() const { return url_; }
    void setUrl(const QString& v) { url_ = v; }

    const QString& description() const { return description_; }
    void setDescription(const QString& v) { description_ = v; }

private:
    int dishId_ = 0;
    QString name_;
    double price_ = 0.0;
    QString category_;
    double rating_ = 5.0;
    QString url_;
    QString description_;
};

} // namespace db
