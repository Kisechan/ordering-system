#include "server.h"

#include <QJsonArray>
#include <QJsonValue>


#include "service/AuthService.h"
#include "service/DishService.h"
#include "service/OrderService.h"
#include "DbManager.h"
#include "entity/DishCount.h"

Server* Server::server = new Server();

Server::Server(QObject* parent)
    : QTcpServer(parent) {
}

Server* Server::getInstance() {
    return server;
}

bool Server::start(quint16 port) {
    if (listen(QHostAddress::Any, port)) {
        qDebug() << "Server started, listening on port" << port;
        return true;
    } else {
        qDebug() << "Failed to start server:" << errorString();
        return false;
    }
}

// 每当有客户端连接
void Server::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket* socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        socket->deleteLater();
        return;
    }

    qDebug() << "New client connected:" << socketDescriptor;

    // 信号槽处理
    connect(socket, &QTcpSocket::readyRead, this, &Server::readData);
    connect(socket, &QTcpSocket::disconnected, this, &Server::disconnected);
}

// 读取客户端数据
void Server::readData() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;

    QByteArray data = socket->readAll();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << parseError.errorString();
        return;
    }

    QJsonObject request = doc.object();
    processRequest(socket, request);
}

// 客户端断开
void Server::disconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;

    qDebug() << "Client disconnected:" << socket;
    userMap.remove(socket);
    socket->deleteLater();
}

// 处理客户端请求
void Server::processRequest(QTcpSocket* socket, const QJsonObject& request) {
    QString type = request["type"].toString();
    QJsonObject response;
    
    // 获取当前线程的数据库连接（线程安全）
    QSqlDatabase conn = db::DbManager::instance().db();
    
    // 检查数据库连接是否有效
    if (!conn.isValid() || !conn.isOpen()) {
        qWarning() << "[Server] Database connection failed for thread" << QThread::currentThread();
        response["code"] = 500;
        response["msg"] = "服务器数据库连接失败";
        sendJson(socket, response);
        return;
    }

    if (type == "login") {
        QString userName = request["username"].toString();
        QString password = request["password"].toString();

        // 获取登录响应结果
        db::AuthService authService(conn);
        response = authService.login(userName, password);

        // 如果登录成功，记录到 userMap
        if (response["code"].toInt() == 200) {
            UserSession userSession;
            if (response.contains("data")) {
                QJsonObject data = response["data"].toObject();
                userSession.userId = data["user_id"].toInt();
                userSession.username = data["username"].toString();
                userMap[socket] = userSession;
                qDebug() << "[Server] User logged in: " << userSession.username << " ID: " << userSession.userId;
            }
        }
    } else if (type == "register") {
        QString username = request["username"].toString();
        QString password = request["password"].toString();

        // 取注册响应结果
        db::AuthService authService(conn);
        response = authService.registerUser(username, password);
    } else if (type == "dish_list") {
        qDebug() << "[Server] ========== 处理菜品列表请求开始 ==========";
        qDebug() << "[Server] 客户端请求菜品列表";
        
        // 获取菜品详细信息
        qDebug() << "[Server] 创建 DishService...";
        db::DishService dishService(conn);
        
        qDebug() << "[Server] 调用 dishService.listAll()...";
        response = dishService.listAll();
        
        qDebug() << "[Server] 响应码:" << response["code"].toInt();
        qDebug() << "[Server] 响应消息:" << response["msg"].toString();
        
        if (response["code"].toInt() == 200) {
            QJsonArray dishes = response.contains("data") ? response["data"].toArray() : QJsonArray();
            qDebug() << "[Server] 菜品列表获取成功，返回菜品数:" << dishes.size();
        } else {
            qWarning() << "[Server] 菜品列表获取失败:" << response["msg"].toString();
        }
        qDebug() << "[Server] ========== 处理菜品列表请求结束 ==========";
        response["type"] = type;  // 回复相同的 type
    } else if (type == "call_waiter") {
        // 发送呼叫服务员信号
        QString username = userMap[socket].username;
        emit callWaiter(username);
        response["code"] = 200;
        response["msg"] = "服务员已收到您的呼叫，请耐心等待~";
    } else if (type == "order_submit") {
        int userId = userMap[socket].userId;
        QVector<db::DishCount> dishCountList;
        QJsonArray data = request["data"].toArray();
        for (QJsonValue v : data) {
            QJsonObject obj = v.toObject();
            db::DishCount ds;
            ds.setDishId(obj["dish_id"].toInt());
            ds.setCount(obj["count"].toInt());
            dishCountList.append(ds);
        }

        // 获取订单提交结果
        db::OrderService orderService(conn);
        response = orderService.submitOrder(userId, dishCountList);
        response["type"] = type;  // 回复相同的 type
    } else if (type == "order_list") {
        qDebug() << "[Server] ========== 处理订单列表请求开始 ==========";
        int userId = userMap[socket].userId;
        qDebug() << "[Server] 客户端请求订单列表 (userId=" << userId << ")";

        // 获取订单详细信息
        qDebug() << "[Server] 创建 OrderService...";
        db::OrderService orderService(conn);
        
        qDebug() << "[Server] 调用 orderService.listOrdersByUser()...";
        response = orderService.listOrdersByUser(userId);
        
        qDebug() << "[Server] 响应码:" << response["code"].toInt();
        qDebug() << "[Server] 响应消息:" << response["msg"].toString();
        
        if (response["code"].toInt() == 200) {
            QJsonArray orders = response.contains("data") ? response["data"].toArray() : QJsonArray();
            qDebug() << "[Server] 订单列表获取成功，返回订单数:" << orders.size();
        } else {
            qWarning() << "[Server] 订单列表获取失败:" << response["msg"].toString();
        }
        qDebug() << "[Server] ========== 处理订单列表请求结束 ==========";
        response["type"] = type;  // 回复相同的 type
    } else if (type == "order_comment") {
        qDebug() << "[Server] ========== 处理订单评价请求开始 ==========";
        QJsonObject data = request["data"].toObject();
        int orderId = data["order_id"].toInt();  // 修改：orderId -> order_id
        QString comment = data["comment"].toString();
        QJsonArray dishList = data["dishes"].toArray();  // 修改：data["data"] -> data["dishes"]
        
        qDebug() << "[Server] 客户端提交订单评价 (orderId=" << orderId << ")";
        qDebug() << "[Server] 评论内容:" << comment;
        qDebug() << "[Server] 评分菜品数量:" << dishList.size();
        
        QVector<int> dishIdList;
        QVector<int> dishRatingList;

        for (QJsonValue v : dishList) {
            QJsonObject object = v.toObject();
            int dishId = object["dish_id"].toInt();
            int rating = object["rating"].toInt();
            dishIdList.append(dishId);
            dishRatingList.append(rating);
            qDebug() << "[Server]   菜品 ID:" << dishId << ", 评分:" << rating;
        }

        qDebug() << "[Server] 创建 OrderService...";
        db::OrderService orderService(conn);
        
        qDebug() << "[Server] 调用 orderService.submitComment()...";
        response = orderService.submitComment(orderId, comment, dishIdList, dishRatingList);
        
        qDebug() << "[Server] 响应码:" << response["code"].toInt();
        qDebug() << "[Server] 响应消息:" << response["msg"].toString();
        
        if (response["code"].toInt() == 200) {
            qDebug() << "[Server] 订单评价提交成功";
        } else {
            qWarning() << "[Server] 订单评价提交失败:" << response["msg"].toString();
        }
        qDebug() << "[Server] ========== 处理订单评价请求结束 ==========";
        response["type"] = type;  // 回复相同的 type
    } else {
        response["code"] = 401;
        response["msg"] = "未知的请求!";
        response["type"] = type;  // 回复相同的 type
    }

    qDebug() << "[Server] ========== 准备发送响应 ==========";
    qDebug() << "[Server] 响应类型:" << type;
    qDebug() << "[Server] Socket 指针:" << socket;
    qDebug() << "[Server] 响应码:" << response["code"].toInt();
    qDebug() << "[Server] 响应消息:" << response["msg"].toString();
    
    sendJson(socket, response);
    
    qDebug() << "[Server] ========== 响应已发送 ==========";
}

// 发送 JSON 给客户端
void Server::sendJson(QTcpSocket* socket, const QJsonObject& obj) {
    if (!socket) {
        qWarning() << "[Server::sendJson] Socket 为空，无法发送";
        return;
    }
    
    qDebug() << "[Server::sendJson] 开始发送 JSON...";
    
    QJsonDocument doc(obj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    
    // 添加换行符作为消息分隔符
    jsonData.append("\n");
    
    qDebug() << "[Server::sendJson] JSON 数据大小:" << jsonData.size() << "字节 (包含换行符)";
    qDebug() << "[Server::sendJson] JSON 内容:" << QString(jsonData.trimmed());
    
    qint64 bytesWritten = socket->write(jsonData);
    qDebug() << "[Server::sendJson] 已写入字节数:" << bytesWritten;
    
    bool flushResult = socket->flush();
    qDebug() << "[Server::sendJson] Flush 结果:" << (flushResult ? "成功" : "失败");
    
    if (bytesWritten < 0) {
        qWarning() << "[Server::sendJson] 写入失败:" << socket->errorString();
    }
}
