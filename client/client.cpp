#include "client.h"
#include <QJsonDocument>
#include <QDebug>
#include <QtGlobal>

Client* Client::client = new Client();

Client::Client(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    receiveBuffer.clear();  // 初始化接收缓冲区

    qDebug() << "[Client::Client] 初始化 Client，连接信号...";
    
    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    qDebug() << "[Client::Client] connected 信号已连接";
    
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    qDebug() << "[Client::Client] disconnected 信号已连接";
    
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    qDebug() << "[Client::Client] readyRead 信号已连接 - 这是关键！";
    
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onErrorOccurred(QAbstractSocket::SocketError)));
    qDebug() << "[Client::Client] error 信号已连接";
    
    qDebug() << "[Client::Client] Client 初始化完成";
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
    qDebug() << "[Client::connectToServer] 开始连接到" << host << ":" << port;
    qDebug() << "[Client::connectToServer] 当前 Socket 状态:" << socket->state();
    emit logMessage(QString("Connecting to %1:%2 ...").arg(host).arg(port));
    socket->connectToHost(host, port);
    qDebug() << "[Client::connectToServer] connectToHost() 调用完成，Socket 状态:" << socket->state();
}

// 发送 JSON 数据
void Client::sendJson(const QJsonObject &obj) {
    qDebug() << "[Client::sendJson] 开始发送 JSON...";
    qDebug() << "[Client::sendJson] Socket 状态:" << socket->state();
    qDebug() << "[Client::sendJson] Socket 已连接:" << (socket->state() == QTcpSocket::ConnectedState ? "是" : "否");
    
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    qDebug() << "[Client::sendJson] JSON 数据大小:" << data.size() << "字节";
    qDebug() << "[Client::sendJson] JSON 内容:" << QString(data);
    
    qint64 bytesWritten = socket->write(data);
    qDebug() << "[Client::sendJson] 已写入字节数:" << bytesWritten;
    
    socket->flush();
    qDebug() << "[Client::sendJson] Flush 完成";
    
    if (bytesWritten < 0) {
        qWarning() << "[Client::sendJson] 写入失败:" << socket->errorString();
    }
    
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
    qDebug() << "[Client::onConnected] ========== 连接成功 ==========";
    qDebug() << "[Client::onConnected] Socket 状态:" << socket->state();
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
    qDebug() << "[Client::onReadyRead] ========== 数据到达 ==========";
    qDebug() << "[Client::onReadyRead] Socket 状态:" << socket->state();
    qDebug() << "[Client::onReadyRead] 可读字节数:" << socket->bytesAvailable();
    
    // 读取所有可用数据到缓冲区
    receiveBuffer.append(socket->readAll());
    qDebug() << "[Client::onReadyRead] 缓冲区当前大小:" << receiveBuffer.size() << "字节";
    
    // 按行处理JSON消息（每条消息以\n结尾）
    while (receiveBuffer.contains('\n')) {
        int newlinePos = receiveBuffer.indexOf('\n');
        QByteArray jsonLine = receiveBuffer.left(newlinePos).trimmed();
        receiveBuffer.remove(0, newlinePos + 1);
        
        if (jsonLine.isEmpty()) {
            qDebug() << "[Client::onReadyRead] 跳过空行";
            continue;
        }
        
        qDebug() << "[Client::onReadyRead] 处理一条消息，大小:" << jsonLine.size() << "字节";
        qDebug() << "[Client::onReadyRead] 消息内容:" << QString::fromUtf8(jsonLine);
        
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonLine, &parseError);
        
        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "[Client::onReadyRead] JSON 解析错误:" << parseError.errorString();
            emit logMessage(QString("JSON parse error: %1").arg(parseError.errorString()));
            continue;
        }
        
        qDebug() << "[Client::onReadyRead] JSON 解析成功";
        
        QJsonObject obj = doc.object();
        qDebug() << "[Client::onReadyRead] 发送 receivedJson 信号";
        
        emit receivedJson(obj);
        emit logMessage(QString("Received JSON: %1").arg(QString(jsonLine)));
    }
    
    if (!receiveBuffer.isEmpty()) {
        qDebug() << "[Client::onReadyRead] 缓冲区还有数据等待下一次 readyRead:" << receiveBuffer.size() << "字节";
    }
    
    qDebug() << "[Client::onReadyRead] ========== 数据处理完成 ==========";
}

// 错误处理
void Client::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    qWarning() << "[Client::onErrorOccurred] ========== Socket 错误发生 ==========";
    qWarning() << "[Client::onErrorOccurred] 错误类型:" << socketError;
    qWarning() << "[Client::onErrorOccurred] 错误字符串:" << socket->errorString();
    emit logMessage(QString("Socket error: %1").arg(socket->errorString()));
    emit connectionError(socket->errorString());
}
