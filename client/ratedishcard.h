#pragma once
#include <QWidget>

#include "carttype.h"

class ElaImageCard;
class ElaText;
class QSlider;

class RateDishCard : public QWidget
{
    Q_OBJECT
public:
    explicit RateDishCard(QWidget* parent = nullptr);

    void setItem(const CartItem& item);
    void setRating(int rating);  // 设置初始评分
    int  rating() const { return m_rating; }
    int  dishId() const { return m_item.dish.dish_id; }

signals:
    void ratingChanged(int dishId, int rating);

private slots:
    void onSliderChanged(int v);

private:
    void refreshUI();

private:
    CartItem m_item;
    int m_rating = 5;

    ElaImageCard* m_img = nullptr;
    ElaText* m_title = nullptr;
    ElaText* m_meta = nullptr;
    ElaText* m_desc = nullptr;
    ElaText* m_qtyAndTotal = nullptr;

    ElaText* m_rateText = nullptr;
    QSlider* m_slider = nullptr;
};
