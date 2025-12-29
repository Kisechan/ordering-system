#include "client.h"
#include <QJsonDocument>
#include <QDebug>
#include <QtGlobal>

Client* Client::client = new Client();

Client::Client(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onErrorOccurred(QAbstractSocket::SocketError)));
}

Client* Client::getInstance() {
    return client;
}

void Client::destroyInstance() {
    if (client) {
        client->deleteLater();
        client = nullptr;
    }
}

// 连接到服务器
void Client::connectToServer(const QString &host, quint16 port) {
    emit logMessage(QString("Connecting to %1:%2 ...").arg(host).arg(port));
    socket->connectToHost(host, port);
}

// 发送 JSON 数据
void Client::sendJson(const QJsonObject &obj) {
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    socket->write(data);
    socket->flush();
    emit logMessage(QString("Sent JSON: %1").arg(QString(data)));
}

bool Client::isConnected() const {
    return socket->state() == QTcpSocket::ConnectedState;
}

void Client::disconnectFromServer() {
    if (socket->state() != QTcpSocket::UnconnectedState) {
        socket->disconnectFromHost();
    }
}

// 连接成功
void Client::onConnected() {
    emit logMessage("Connected to server");
    emit connected();
}

// 断开连接
void Client::onDisconnected() {
    emit logMessage("Disconnected from server");
    emit disconnected();
}

// 接收数据
void Client::onReadyRead() {
    QByteArray data = socket->readAll();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        emit logMessage(QString("JSON parse error: %1").arg(parseError.errorString()));
        return;
    }

    QJsonObject obj = doc.object();
    emit receivedJson(obj);
    emit logMessage(QString("Received JSON: %1").arg(QString(data)));
}

// 错误处理
void Client::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    emit logMessage(QString("Socket error: %1").arg(socket->errorString()));
    emit connectionError(socket->errorString());
}
