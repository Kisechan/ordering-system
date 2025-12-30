#pragma once
#include <QDialog>
#include <QMap>

#include "carttype.h"

class ElaPlainTextEdit;
class ElaScrollArea;
class QVBoxLayout;
class QWidget;

class ReviewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReviewDialog(int orderId,
                          double totalAmount,
                          const QString& time,
                          const QList<CartItem>& items,
                          const QString& presetComment = QString(),
                          QWidget* parent = nullptr);

    QString comment() const;                 // 提交后的评论
    QMap<int, int> ratings() const;          // dish_id -> 1..5

private:
    void buildUI();

private:
    int m_orderId = 0;
    double m_totalAmount = 0.0;
    QString m_time;
    QList<CartItem> m_items;

    ElaPlainTextEdit* m_edit = nullptr;

    ElaScrollArea* m_scroll = nullptr;
    QWidget* m_listContainer = nullptr;
    QVBoxLayout* m_listLayout = nullptr;

    QMap<int, int> m_ratings; // dish_id -> rating
};
