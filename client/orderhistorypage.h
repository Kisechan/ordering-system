#pragma once
#include "ElaScrollPage.h"
#include "RateDialog.h"

#include <QList>
#include <QString>

#include "carttype.h"

class QTimer;
class QWidget;
class QVBoxLayout;
class NetworkManager;

class OrderCard;

class OrderHistoryPage : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit OrderHistoryPage(NetworkManager* networkMgr, QWidget* parent = nullptr);

private slots:
    void onOrderListReceived(const QJsonArray& orders);
    void onOrderListError(const QString& error);
    void onOrderCommentSuccess();
    void onOrderCommentFailed(const QString& error);

private:
    void rebuildList();
    void clearList();
    void showEmpty(const QString& text);

    QList<int> m_orderIds;
    QList<double> m_totalAmounts;
    QList<QString> m_times;
    QList<QString> m_comments;
    QList<QList<CartItem>> m_orderItems;
    QList<QMap<int, int>> m_dishRatings;  // 每个订单的菜品评分 (dish_id -> rating)

private:
    NetworkManager* m_networkMgr = nullptr;

    QWidget* m_content = nullptr;
    QWidget* m_listContainer = nullptr;
    QVBoxLayout* m_listLayout = nullptr;
};