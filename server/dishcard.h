#pragma once
#ifndef DISHCARD_H
#define DISHCARD_H

#include <QWidget>
#include <QString>

class ElaImageCard;
class ElaText;
class ElaPushButton;

struct Dish {
    int     dish_id = 0;
    QString name;
    double  price = 0.0;
    QString category;
    double  rating = 0.0;
    QString url= QStringLiteral(":/Image/vvan.jpg");
    QString description;
};


class DishCard : public QWidget
{
    Q_OBJECT
public:
    explicit DishCard(QWidget* parent = nullptr);

    void setDish(const Dish& d);
    Dish dish() const { return m_dish; }

signals:
    void editRequested(int dishId);
    void deleteRequested(int dishId);

private slots:
    void onEdit();
    void onDelete();

private:
    void refreshUI();

private:
    Dish m_dish;

    ElaImageCard*   m_img = nullptr;

    ElaText*        m_title = nullptr;
    ElaText*        m_category = nullptr;  // 品类
    ElaText*        m_rating = nullptr;    // 评分
    ElaText*        m_price = nullptr;     // 价格
    ElaText*        m_desc  = nullptr;     // 描述
    ElaText*        m_dishId = nullptr;    // 菜品ID

    ElaPushButton*  m_editBtn = nullptr;   // 修改按钮
    ElaPushButton*  m_deleteBtn = nullptr; // 删除按钮
};

#endif // DISHCARD_H
