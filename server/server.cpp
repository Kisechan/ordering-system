#include "server.h"

#include <QJsonArray>
#include <QJsonValue>

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

    if (type == "login") {
        QString userName = request["username"].toString();
        QString password = request["password"].toString();

        // TODO: 获取登录响应结果

        // TODO: 如果登录成功，记录到 userMap
    } else if (type == "register") {
        QString username = request["username"].toString();
        QString password = request["password"].toString();

        // TODO: 获取注册响应结果
    } else if (type == "dish_list") {
        // TODO: 获取菜品详细信息
    } else if (type == "call_waiter") {
        // 发送呼叫服务员信号
        QString username = userMap[socket].username;
        emit callWaiter(username);
    } else if (type == "order_submit") {
        int userId = userMap[socket].userId;
        QJsonObject dataObj = request["data"].toObject();
        QJsonArray dishesArray = dataObj["dishes"].toArray();

        QList<int> dishIdList, countList;
        for (const QJsonValue& v : dishesArray) {
            QJsonObject dishObj = v.toObject();
            dishIdList.append(dishObj["dish_id"].toInt());
            countList.append(dishObj["count"].toInt());
        }

        // TODO: 获取订单提交结果
    } else if (type == "order_list") {
        int uid = userMap[socket].userId;
        // TODO: 获取订单详细信息
    } else if (type == "order_comment") {
        int uid = userMap[socket].userId;
        // TODO: 获取提交评论结果
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
