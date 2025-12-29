#pragma once

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
    QString url;          // 资源路径(:/...) 或后端路径(/img/xxx.jpg)
    QString description;
};

class DishCard : public QWidget
{
    Q_OBJECT
public:
    explicit DishCard(QWidget* parent = nullptr);

    void setDish(const Dish& d);
    Dish dish() const { return m_dish; }

    int quantity() const { return m_qty; }
    void setQuantity(int q);

signals:
    void addToCartRequested(int dishId, int quantity);
    void quantityChanged(int dishId, int quantity);

private slots:
    void onMinus();
    void onPlus();
    void onAddToCart();

private:
    void refreshUI();

private:
    Dish m_dish;
    int  m_qty = 1;

    ElaImageCard*   m_img = nullptr;

    ElaText*        m_title = nullptr;
    ElaText*        m_meta  = nullptr;   // 品类 + 评分
    ElaText*        m_desc  = nullptr;

    ElaPushButton*  m_minus = nullptr;
    ElaText*        m_qtyText = nullptr;
    ElaPushButton*  m_plus  = nullptr;

    ElaText*        m_price = nullptr;
    ElaPushButton*  m_addBtn = nullptr;
};
