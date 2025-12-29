#include "server.h"

#include <QJsonArray>
#include <QJsonValue>


#include "service/AuthService.h"
#include "service/DishService.h"
#include "service/OrderService.h"
#include "DbManager.h"
#include "entity/DishCount.h"

Server* Server::server = new Server();
QSqlDatabase Server::conn;

Server::Server(QObject* parent)
    : QTcpServer(parent) {
    initConnection();
}

void Server::initConnection() {
    db::DbConfig cfg;
    cfg.odbcConnStr =
        "DRIVER={MariaDB ODBC 3.2 Driver};"
        "TCPIP=1;"
        "SERVER=localhost;"
        "PORT=3306;"
        "DATABASE=restaurant;"
        "UID=root;"
        "PWD=123456;"
        "OPTION=3;";

    auto r = db::DbManager::instance().init(cfg);
    if (!r.isOk()) {
        qCritical() << "DB init failed:" << r.message;
    }
    conn = db::DbManager::instance().db();
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

    if (type == "login") {
        QString userName = request["username"].toString();
        QString password = request["password"].toString();

        // 获取登录响应结果
        db::AuthService authService(conn);
        response = authService.login(userName, password);

        // 如果登录成功，记录到 userMap
        UserSession userSession;
        QJsonObject data = request["data"].toObject();
        userSession.userId = data["user_id"].toInt();
        userSession.username = data["username"].toString();
        userMap[socket] = userSession;
    } else if (type == "register") {
        QString username = request["username"].toString();
        QString password = request["password"].toString();

        // 取注册响应结果
        db::AuthService authService(conn);
        response = authService.registerUser(username, password);
    } else if (type == "dish_list") {
        // 获取菜品详细信息
        db::DishService dishService(conn);
        response = dishService.listAll();
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
    } else if (type == "order_list") {
        int userId = userMap[socket].userId;

        // 获取订单详细信息
        db::OrderService orderService(conn);
        response = orderService.listOrdersByUser(userId);
    } else if (type == "order_comment") {
        QJsonObject data = request["data"].toObject();
        int orderId = data["orderId"].toInt();
        QString comment = data["comment"].toString();

        // 获取提交评论结果
        db::OrderService orderService(conn);
        response = orderService.submitComment(orderId, comment);
    } else {
        response["code"] = 401;
        response["msg"] = "未知的请求!";
    }

    sendJson(socket, response);
}

// 发送 JSON 给客户端
void Server::sendJson(QTcpSocket* socket, const QJsonObject& obj) {
    QJsonDocument doc(obj);
    socket->write(doc.toJson(QJsonDocument::Compact));
    socket->flush();
}
