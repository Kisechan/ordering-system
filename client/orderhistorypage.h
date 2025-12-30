#pragma once
#include "ElaScrollPage.h"

#include <QList>
#include <QString>

#include "carttype.h"

class QTimer;
class QWidget;
class QVBoxLayout;

class OrderCard;

class OrderHistoryPage : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit OrderHistoryPage(const QString& userId, QWidget* parent = nullptr);

signals:
    // 预留后端接口：只用 userId 去请求订单列表
    void ordersRequested(const QString& userId);

    // 点“保存评论”后，交给后端更新
    void updateCommentRequested(const QString& userId, int orderId, const QString& newComment);

public slots:
    // 后端把“该用户订单列表”塞进来（json 是数组：[{order_id,...,dishes:[...]}]）
    void setOrdersJson(const QByteArray& json);

    // 后端更新评论后回调（可选）
    void applyCommentUpdateResult(int orderId, bool ok, const QString& message);

private:
    void rebuildList();
    void clearList();
    void showEmpty(const QString& text);

    QList<int> m_orderIds;
    QList<double> m_totalAmounts;
    QList<QString> m_times;
    QList<QString> m_comments;
    QList<QList<CartItem>> m_orderItems;

private:
    QString m_userId;

    QWidget* m_content = nullptr;
    QWidget* m_listContainer = nullptr;
    QVBoxLayout* m_listLayout = nullptr;
};
