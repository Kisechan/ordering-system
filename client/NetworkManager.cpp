#include "NetworkManager.h"
#include "Protocol.h"
#include "NetworkConfig.h"

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent), 
      m_client(Client::getInstance()),
      m_lastRequestType("") {

    // 连接 Client 信号
    connect(m_client, &Client::receivedJson, this, &NetworkManager::onJsonReceived);
}

NetworkManager::~NetworkManager() {
}

void NetworkManager::connectToServer(const QString& ip, quint16 port) {
    m_client->connectToServer(ip, port);
}

void NetworkManager::disconnectFromServer() {
    m_client->disconnectFromServer();
}

bool NetworkManager::isConnected() const {
    return m_client->isConnected();
}

void NetworkManager::login(const QString& username, const QString& password) {
    if (!isConnected()) {
        emit loginFailed("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::LOGIN;
    QJsonObject request = Protocol::buildLoginRequest(username, password);
    m_client->sendJson(request);
}

void NetworkManager::registerUser(const QString& username, const QString& password) {
    if (!isConnected()) {
        emit registerFailed("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::REGISTER;
    QJsonObject request = Protocol::buildRegisterRequest(username, password);
    m_client->sendJson(request);
}

void NetworkManager::getDishList() {
    if (!isConnected()) {
        emit dishListError("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::DISH_LIST;
    QJsonObject request = Protocol::buildDishListRequest();
    m_client->sendJson(request);
}

void NetworkManager::submitOrder(const QJsonArray& dishes) {
    if (!isConnected()) {
        emit orderSubmitFailed("未连接到服务器");
        return;
    }
    if (m_userId == -1) {
        emit orderSubmitFailed("未登录，无法提交订单");
        return;
    }
    m_lastRequestType = Protocol::ORDER_SUBMIT;
    QJsonObject request = Protocol::buildOrderSubmitRequest(dishes);
    m_client->sendJson(request);
}

void NetworkManager::getOrderList() {
    if (!isConnected()) {
        emit orderListError("未连接到服务器");
        return;
    }
    if (m_userId == -1) {
        emit orderListError("未登录，无法获取订单列表");
        return;
    }
    m_lastRequestType = Protocol::ORDER_LIST;
    QJsonObject request = Protocol::buildOrderListRequest();
    m_client->sendJson(request);
}

void NetworkManager::submitOrderComment(int orderId, const QString& comment) {
    if (!isConnected()) {
        emit orderCommentFailed("未连接到服务器");
        return;
    }
    if (m_userId == -1) {
        emit orderCommentFailed("未登录，无法提交评价");
        return;
    }
    m_lastRequestType = Protocol::ORDER_COMMENT;
    QJsonObject request = Protocol::buildOrderCommentRequest(orderId, comment);
    m_client->sendJson(request);
}

void NetworkManager::callWaiter() {
    if (!isConnected()) {
        emit waiterCallError("未连接到服务器");
        return;
    }
    m_lastRequestType = Protocol::CALL_WAITER;
    QJsonObject request = Protocol::buildCallWaiterRequest();
    m_client->sendJson(request);
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
        // 从响应中提取 user_id 并保存到会话
        if (response.data.contains("user_id")) {
            m_userId = response.data.value("user_id").toInt();
        }
        // 如果响应包含 username，也保存
        if (response.data.contains("username")) {
            m_username = response.data.value("username").toString();
        }
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
