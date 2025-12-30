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
        qWarning() << "[NetworkManager] 未连接到服务器";
        emit dishListError("未连接到服务器");
        return;
    }
    qDebug() << "[NetworkManager] 正在请求菜品列表...";
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
        qWarning() << "[NetworkManager] 未连接到服务器";
        emit orderListError("未连接到服务器");
        return;
    }
    if (m_userId == -1) {
        qWarning() << "[NetworkManager] 未登录，无法获取订单列表";
        emit orderListError("未登录，无法获取订单列表");
        return;
    }
    qDebug() << "[NetworkManager] 正在请求订单列表... (userId=" << m_userId << ")";
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

void NetworkManager::submitOrderCommentWithRatings(int orderId, const QString& comment, const QJsonArray& dishes) {
    if (!isConnected()) {
        emit orderCommentFailed("未连接到服务器");
        return;
    }
    if (m_userId == -1) {
        emit orderCommentFailed("未登录，无法提交评价");
        return;
    }
    qDebug() << "[NetworkManager] 提交订单评价 - orderId:" << orderId 
             << ", comment:" << comment 
             << ", dishes count:" << dishes.size();
    m_lastRequestType = Protocol::ORDER_COMMENT;
    QJsonObject request = Protocol::buildOrderCommentRequest(orderId, comment, dishes);
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

    // 调试输出：打印完整的响应数据
    qDebug() << "[NetworkManager] ========== 收到响应 ==========";
    
    // 从响应本身获取 type，而不是依赖 m_lastRequestType
    QString responseType = json.value("type").toString();
    qDebug() << "[NetworkManager] ResponseType (from JSON):" << responseType;
    qDebug() << "[NetworkManager] m_lastRequestType (old):" << m_lastRequestType;
    
    qDebug() << "[NetworkManager] Response Code:" << response.code;
    qDebug() << "[NetworkManager] Response Message:" << response.message;
    qDebug() << "[NetworkManager] Response Valid:" << response.isValid;
    
    // 打印响应数据的键
    if (!response.data.isEmpty()) {
        qDebug() << "[NetworkManager] Response Data Keys:" << response.data.keys();
        if (response.data.contains("array")) {
            QJsonArray arr = response.data.value("array").toArray();
            qDebug() << "[NetworkManager] Array Size:" << arr.size();
        }
    }
    qDebug() << "[NetworkManager] ===========================";

    if (!response.isValid) {
        emit connectionError("服务器响应格式无效");
        return;
    }

    // 优先使用响应中的 type，其次才用 m_lastRequestType
    QString requestType = responseType.isEmpty() ? m_lastRequestType : responseType;
    
    // 根据请求类型分发响应
    if (requestType == Protocol::LOGIN) {
        processLoginResponse(response);
    } else if (requestType == Protocol::REGISTER) {
        processRegisterResponse(response);
    } else if (requestType == Protocol::DISH_LIST) {
        processDishListResponse(response);
    } else if (requestType == Protocol::ORDER_SUBMIT) {
        processOrderSubmitResponse(response);
    } else if (requestType == Protocol::ORDER_LIST) {
        processOrderListResponse(response);
    } else if (requestType == Protocol::ORDER_COMMENT) {
        processOrderCommentResponse(response);
    } else if (requestType == Protocol::CALL_WAITER) {
        processCallWaiterResponse(response);
    }

    // 只在使用的是 responseType 时才清空 m_lastRequestType
    if (!responseType.isEmpty()) {
        m_lastRequestType = "";
    }
}

void NetworkManager::onConnectionError(const QString& error) {
    emit connectionError(error);
}
void NetworkManager::processLoginResponse(const ResponseParser::Response& response) {
    qDebug() << "[NetworkManager::processLoginResponse] ========== 处理登录响应 ==========";
    qDebug() << "[NetworkManager::processLoginResponse] Response Code:" << response.code;
    qDebug() << "[NetworkManager::processLoginResponse] Response Message:" << response.message;
    
    if (ResponseParser::isSuccess(response)) {
        // 从响应中提取 user_id 并保存到会话
        if (response.data.contains("user_id")) {
            m_userId = response.data.value("user_id").toInt();
        }
        // 如果响应包含 username，也保存
        if (response.data.contains("username")) {
            m_username = response.data.value("username").toString();
        }
        qDebug() << "[NetworkManager::processLoginResponse] 登录成功，userId:" << m_userId << ", username:" << m_username;
        emit loginSuccess();
    } else {
        qWarning() << "[NetworkManager::processLoginResponse] 登录失败:" << response.message;
        emit loginFailed(response.message);
    }
    
    qDebug() << "[NetworkManager::processLoginResponse] ===========================";
}

void NetworkManager::processRegisterResponse(const ResponseParser::Response& response) {
    qDebug() << "[NetworkManager::processRegisterResponse] ========== 处理注册响应 ==========";
    qDebug() << "[NetworkManager::processRegisterResponse] Response Code:" << response.code;
    qDebug() << "[NetworkManager::processRegisterResponse] Response Message:" << response.message;
    
    if (ResponseParser::isSuccess(response)) {
        qDebug() << "[NetworkManager::processRegisterResponse] 注册成功";
        emit registerSuccess();
    } else {
        qWarning() << "[NetworkManager::processRegisterResponse] 注册失败:" << response.message;
        emit registerFailed(response.message);
    }
    
    qDebug() << "[NetworkManager::processRegisterResponse] ===========================";
}
void NetworkManager::processDishListResponse(const ResponseParser::Response& response) {
    qDebug() << "[NetworkManager::processDishListResponse] ========== 处理菜品列表响应 ==========";
    qDebug() << "[NetworkManager::processDishListResponse] Response Code:" << response.code;
    qDebug() << "[NetworkManager::processDishListResponse] Response Message:" << response.message;
    
    if (ResponseParser::isSuccess(response)) {
        QJsonArray dishes;
        if (response.data.contains("array") && response.data.value("array").isArray()) {
            dishes = response.data.value("array").toArray();
        }
        qDebug() << "[NetworkManager::processDishListResponse] 菜品列表接收成功，菜品数量:" << dishes.size();
        emit dishListReceived(dishes);
    } else {
        qWarning() << "[NetworkManager::processDishListResponse] 菜品列表接收失败:" << response.message;
        emit dishListError(response.message);
    }
    
    qDebug() << "[NetworkManager::processDishListResponse] ===========================";
}

void NetworkManager::processOrderSubmitResponse(const ResponseParser::Response& response) {
    qDebug() << "[NetworkManager::processOrderSubmitResponse] ========== 处理订单提交响应 ==========";
    qDebug() << "[NetworkManager::processOrderSubmitResponse] Response Code:" << response.code;
    qDebug() << "[NetworkManager::processOrderSubmitResponse] Response Message:" << response.message;
    
    if (ResponseParser::isSuccess(response)) {
        qDebug() << "[NetworkManager::processOrderSubmitResponse] 订单提交成功";
        emit orderSubmitSuccess();
    } else {
        qWarning() << "[NetworkManager::processOrderSubmitResponse] 订单提交失败:" << response.message;
        emit orderSubmitFailed(response.message);
    }
    
    qDebug() << "[NetworkManager::processOrderSubmitResponse] ===========================";
}

void NetworkManager::processOrderListResponse(const ResponseParser::Response& response) {
    qDebug() << "[NetworkManager::processOrderListResponse] ========== 处理订单列表响应 ==========";
    qDebug() << "[NetworkManager::processOrderListResponse] Response Code:" << response.code;
    qDebug() << "[NetworkManager::processOrderListResponse] Response Message:" << response.message;
    
    if (ResponseParser::isSuccess(response)) {
        QJsonArray orders;
        if (response.data.contains("array") && response.data.value("array").isArray()) {
            orders = response.data.value("array").toArray();
        }
        qDebug() << "[NetworkManager::processOrderListResponse] 订单列表接收成功，订单数量:" << orders.size();
        emit orderListReceived(orders);
    } else {
        qWarning() << "[NetworkManager::processOrderListResponse] 订单列表接收失败:" << response.message;
        emit orderListError(response.message);
    }
    
    qDebug() << "[NetworkManager::processOrderListResponse] ===========================";
}

void NetworkManager::processOrderCommentResponse(const ResponseParser::Response& response) {
    qDebug() << "[NetworkManager::processOrderCommentResponse] ========== 处理订单评价响应 ==========";
    qDebug() << "[NetworkManager::processOrderCommentResponse] Response Code:" << response.code;
    qDebug() << "[NetworkManager::processOrderCommentResponse] Response Message:" << response.message;
    
    if (ResponseParser::isSuccess(response)) {
        qDebug() << "[NetworkManager::processOrderCommentResponse] 订单评价提交成功";
        emit orderCommentSuccess();
    } else {
        qWarning() << "[NetworkManager::processOrderCommentResponse] 订单评价提交失败:" << response.message;
        emit orderCommentFailed(response.message);
    }
    
    qDebug() << "[NetworkManager::processOrderCommentResponse] ===========================";
}

void NetworkManager::processCallWaiterResponse(const ResponseParser::Response& response) {
    qDebug() << "[NetworkManager::processCallWaiterResponse] ========== 处理呼叫服务员响应 ==========";
    qDebug() << "[NetworkManager::processCallWaiterResponse] Response Code:" << response.code;
    qDebug() << "[NetworkManager::processCallWaiterResponse] Response Message:" << response.message;
    
    if (ResponseParser::isSuccess(response)) {
        qDebug() << "[NetworkManager::processCallWaiterResponse] 呼叫服务员成功";
        emit waiterCalled();
    } else {
        qWarning() << "[NetworkManager::processCallWaiterResponse] 呼叫服务员失败:" << response.message;
        emit waiterCallError(response.message);
    }
    
    qDebug() << "[NetworkManager::processCallWaiterResponse] ===========================";
}
