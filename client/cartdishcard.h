#pragma once
#include <QWidget>

#include "carttype.h"

class ElaImageCard;
class ElaText;

class CartDishCard : public QWidget
{
    Q_OBJECT
public:
    explicit CartDishCard(QWidget* parent = nullptr);

    void setItem(const CartItem& item);

private:
    void refreshUI();

private:
    CartItem m_item;

    ElaImageCard* m_img   = nullptr;
    ElaText*      m_title = nullptr;
    ElaText*      m_meta  = nullptr;
    ElaText*      m_desc  = nullptr;

    ElaText*      m_qty   = nullptr;
    ElaText*      m_price = nullptr;
};
