#pragma once
#ifndef DISHINORDERCARD_H
#define DISHINORDERCARD_H

#include <QWidget>
#include "servicerequest.h"

class ElaText;
class ElaImageCard;
class ElaPushButton;

class DishInOrderCard : public QWidget
{
    Q_OBJECT
public:
    explicit DishInOrderCard(QWidget* parent = nullptr);

    void setDishInOrder(const DishInOrder& dishInOrder);
    DishInOrder getDishInOrder() const { return m_dishInOrder; }

signals:
    void servedRequested(const QString& dishName);  // 点击"送达"按钮

private:
    void refreshUI();

private:
    DishInOrder m_dishInOrder;

    ElaImageCard* m_imageCard = nullptr;
    ElaText* m_nameLabel = nullptr;
    ElaText* m_quantityLabel = nullptr;
    ElaPushButton* m_servedBtn = nullptr;
};

#endif // DISHINORDERCARD_H
