#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include "client.h"
#include "ResponseParser.h"

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();

    // 连接到服务器
    void connectToServer(const QString& ip, quint16 port);

    // 断开服务器连接
    void disconnectFromServer();

    // 获取连接状态
    bool isConnected() const;

    // ========== 用户认证相关 ==========
    void login(const QString& username, const QString& password);
    void registerUser(const QString& username, const QString& password);

    // ========== 菜品相关 ==========
    void getDishList();

    // ========== 订单相关 ==========
    void submitOrder(const QJsonArray& dishes);
    void getOrderList();
    void submitOrderComment(int orderId, const QString& comment);

    // ========== 其他 ==========
    void callWaiter();

signals:
    // 连接相关信号
    void connected();
    void disconnected();
    void connectionError(const QString& error);

    // 用户认证相关信号
    void loginSuccess();
    void loginFailed(const QString& error);
    void registerSuccess();
    void registerFailed(const QString& error);

    // 菜品相关信号
    void dishListReceived(const QJsonArray& dishes);
    void dishListError(const QString& error);

    // 订单相关信号
    void orderSubmitSuccess();
    void orderSubmitFailed(const QString& error);
    void orderListReceived(const QJsonArray& orders);
    void orderListError(const QString& error);
    void orderCommentSuccess();
    void orderCommentFailed(const QString& error);

    // 其他信号
    void waiterCalled();
    void waiterCallError(const QString& error);

private slots:
    void onJsonReceived(const QJsonObject& json);
    void onConnectionError(const QString& error);

private:
    void processLoginResponse(const ResponseParser::Response& response);
    void processRegisterResponse(const ResponseParser::Response& response);
    void processDishListResponse(const ResponseParser::Response& response);
    void processOrderSubmitResponse(const ResponseParser::Response& response);
    void processOrderListResponse(const ResponseParser::Response& response);
    void processOrderCommentResponse(const ResponseParser::Response& response);
    void processCallWaiterResponse(const ResponseParser::Response& response);

private:
    Client* m_client;
    QString m_lastRequestType;  // 记录最后一次请求的类型，用于匹配响应
};

#endif // NETWORKMANAGER_H
