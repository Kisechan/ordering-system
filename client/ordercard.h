#pragma once
#include <QWidget>
#include <QString>
#include <QList>

#include "carttype.h"

class ElaText;
class ElaPushButton;

class OrderCard : public QWidget
{
    Q_OBJECT
public:
    explicit OrderCard(QWidget* parent = nullptr);

    // 不引入新结构：直接传基本字段 + 你已有的 QList<CartItem>
    void setOrder(int orderId,
                  double totalAmount,
                  const QString& createTime,
                  const QString& comment,
                  const QList<CartItem>& items);

    int orderId() const { return m_orderId; }
    QString comment() const { return m_comment; }

signals:
    void editCommentRequested(int orderId, const QString& currentComment);
    void rateRequested(int orderId); // 先留按钮/信号

private slots:
    void onEditComment();
    void onRate();

private:
    void refreshUI();

private:
    int m_orderId = 0;
    double m_totalAmount = 0.0;
    QString m_createTime;
    QString m_comment;
    QList<CartItem> m_items; // 用你已有结构承载 dishes+count

    ElaText* m_title = nullptr;     // 订单号
    ElaText* m_time  = nullptr;     // 时间
    ElaText* m_price = nullptr;     // 总价
    ElaText* m_commentText = nullptr; // 评论（可隐藏）

    ElaPushButton* m_commentBtn = nullptr; // 评论(新增/修改)
    ElaPushButton* m_rateBtn    = nullptr; // 评分(预留)
};
