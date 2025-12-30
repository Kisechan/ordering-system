#pragma once
#include "ElaScrollPage.h"

#include "cartmanager.h"

class QWidget;
class QVBoxLayout;
class ElaScrollArea;
class ElaText;
class ElaPushButton;
class NetworkManager;

class CartPage : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit CartPage(CartManager* cart, NetworkManager* networkMgr = nullptr, QWidget* parent = nullptr);

signals:
    // 预留：点“呼叫服务员/下单”
    void orderRequested(const OrderDraft& draft);

private slots:
    void reloadFromCart();
    void onSubmitOrder();
    void onOrderSuccess();
    void onOrderFailed(const QString& error);

private:
    void rebuildList(const QList<CartItem>& items);
    void showMessage(const QString& msg, int duration = 2000);

private:
    CartManager*    m_cart = nullptr;
    NetworkManager* m_networkMgr = nullptr;

    QWidget*      m_listContainer = nullptr;
    QVBoxLayout*  m_listLayout = nullptr;

    // 可选：底部按钮（不想要就删）
    ElaPushButton* m_callBtn = nullptr;
    ElaText*       m_statusText = nullptr;  // 状态提示
};
