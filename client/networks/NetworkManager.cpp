#include "NetworkManager.h"
#include "Protocol.h"
#include "NetworkConfig.h"

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent), 
      m_tcpClient(new TcpClient(this)),
      m_lastRequestType("") {

    // 连接 TcpClient 信号
    connect(m_tcpClient, &TcpClient::connected, this, &NetworkManager::connected);
    connect(m_tcpClient, &TcpClient::disconnected, this, &NetworkManager::disconnected);
    connect(m_tcpClient, &TcpClient::jsonReceived, this, &NetworkManager::onJsonReceived);
    connect(m_tcpClient, &TcpClient::connectionError, this, &NetworkManager::onConnectionError);
}

NetworkManager::~NetworkManager() {
}

void NetworkManager::connectToServer(const QString& ip, quint16 port) {
    m_tcpClient->connectToServer(ip, port, NetworkConfig::TCP_TIMEOUT_MS);
}

void NetworkManager::disconnectFromServer() {
    m_tcpClient->disconnectFromServer();
}

bool NetworkManager::isConnected() const {
    return m_tcpClient->isConnected();
}

void NetworkManager::login(const QString& username, const QString& password) {
    if (!isConnected()) {
        emit loginFailed("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::LOGIN;
    QJsonObject request = Protocol::buildLoginRequest(username, password);
    m_tcpClient->sendJson(request);
}

void NetworkManager::registerUser(const QString& username, const QString& password) {
    if (!isConnected()) {
        emit registerFailed("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::REGISTER;
    QJsonObject request = Protocol::buildRegisterRequest(username, password);
    m_tcpClient->sendJson(request);
}

void NetworkManager::getDishList() {
    if (!isConnected()) {
        emit dishListError("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::DISH_LIST;
    QJsonObject request = Protocol::buildDishListRequest();
    m_tcpClient->sendJson(request);
}

void NetworkManager::submitOrder(const QJsonArray& dishes) {
    if (!isConnected()) {
        emit orderSubmitFailed("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::ORDER_SUBMIT;
    QJsonObject request = Protocol::buildOrderSubmitRequest(dishes);
    m_tcpClient->sendJson(request);
}

void NetworkManager::getOrderList() {
    if (!isConnected()) {
        emit orderListError("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::ORDER_LIST;
    QJsonObject request = Protocol::buildOrderListRequest();
    m_tcpClient->sendJson(request);
}

void NetworkManager::submitOrderComment(int orderId, const QString& comment) {
    if (!isConnected()) {
        emit orderCommentFailed("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::ORDER_COMMENT;
    QJsonObject request = Protocol::buildOrderCommentRequest(orderId, comment);
    m_tcpClient->sendJson(request);
}

void NetworkManager::callWaiter() {
    if (!isConnected()) {
        emit waiterCallError("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::CALL_WAITER;
    QJsonObject request = Protocol::buildCallWaiterRequest();
    m_tcpClient->sendJson(request);
}

void NetworkManager::onJsonReceived(const QJsonObject& json) {
    // 解析响应
    ResponseParser::Response response = ResponseParser::parseResponse(json);

    if (!response.isValid) {
        emit connectionError("服务器响应格式无效");
        return;
    }

    // 根据最后的请求类型分发响应
    if (m_lastRequestType == Protocol::LOGIN) {
        processLoginResponse(response);
    } else if (m_lastRequestType == Protocol::REGISTER) {
        processRegisterResponse(response);
    } else if (m_lastRequestType == Protocol::DISH_LIST) {
        processDishListResponse(response);
    } else if (m_lastRequestType == Protocol::ORDER_SUBMIT) {
        processOrderSubmitResponse(response);
    } else if (m_lastRequestType == Protocol::ORDER_LIST) {
        processOrderListResponse(response);
    } else if (m_lastRequestType == Protocol::ORDER_COMMENT) {
        processOrderCommentResponse(response);
    } else if (m_lastRequestType == Protocol::CALL_WAITER) {
        processCallWaiterResponse(response);
    }

    m_lastRequestType = "";
}

void NetworkManager::onConnectionError(const QString& error) {
    emit connectionError(error);
}

void NetworkManager::processLoginResponse(const ResponseParser::Response& response) {
    if (ResponseParser::isSuccess(response)) {
        emit loginSuccess();
    } else {
        emit loginFailed(response.message);
    }
}

void NetworkManager::processRegisterResponse(const ResponseParser::Response& response) {
    if (ResponseParser::isSuccess(response)) {
        emit registerSuccess();
    } else {
        emit registerFailed(response.message);
    }
}

void NetworkManager::processDishListResponse(const ResponseParser::Response& response) {
    if (ResponseParser::isSuccess(response)) {
        QJsonArray dishes;
        if (response.data.contains("array") && response.data.value("array").isArray()) {
            dishes = response.data.value("array").toArray();
        }
        emit dishListReceived(dishes);
    } else {
        emit dishListError(response.message);
    }
}

void NetworkManager::processOrderSubmitResponse(const ResponseParser::Response& response) {
    if (ResponseParser::isSuccess(response)) {
        emit orderSubmitSuccess();
    } else {
        emit orderSubmitFailed(response.message);
    }
}

void NetworkManager::processOrderListResponse(const ResponseParser::Response& response) {
    if (ResponseParser::isSuccess(response)) {
        QJsonArray orders;
        if (response.data.contains("array") && response.data.value("array").isArray()) {
            orders = response.data.value("array").toArray();
        }
        emit orderListReceived(orders);
    } else {
        emit orderListError(response.message);
    }
}

void NetworkManager::processOrderCommentResponse(const ResponseParser::Response& response) {
    if (ResponseParser::isSuccess(response)) {
        emit orderCommentSuccess();
    } else {
        emit orderCommentFailed(response.message);
    }
}

void NetworkManager::processCallWaiterResponse(const ResponseParser::Response& response) {
    if (ResponseParser::isSuccess(response)) {
        emit waiterCalled();
    } else {
        emit waiterCallError(response.message);
    }
}
