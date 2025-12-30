#pragma once
#include <QDialog>
#include <QMap>

#include "carttype.h"

class QWidget;
class QVBoxLayout;
class ElaScrollArea;

class RateDishCard;

class RateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RateDialog(int orderId,
                        double totalAmount,
                        const QString& time,
                        const QList<CartItem>& items,
                        QWidget* parent = nullptr);

    // dish_id -> rating(1~5)
    QMap<int, int> ratings() const { return m_ratings; }

private:
    void buildUI();

private:
    int m_orderId = 0;
    double m_totalAmount = 0.0;
    QString m_time;
    QList<CartItem> m_items;

    QMap<int, int> m_ratings;

    QWidget* m_content = nullptr;
    QWidget* m_listContainer = nullptr;
    QVBoxLayout* m_listLayout = nullptr;
    ElaScrollArea* m_scroll = nullptr;
};
