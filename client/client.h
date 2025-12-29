#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class Client : public QObject
{
    Q_OBJECT
public:
    static Client* getInstance();

    static void destroyInstance();

    void connectToServer(const QString &host, quint16 port);

    void sendJson(const QJsonObject &obj);

    // 获取连接状态
    bool isConnected() const;

    // 主动断开连接
    void disconnectFromServer();

signals:
    void logMessage(const QString &msg);

    void receivedJson(const QJsonObject &obj);

    void connected();

    void disconnected();

    void connectionError(const QString &error);

private slots:
    void onReadyRead();

    void onConnected();

    void onDisconnected();

    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;

    static Client* client;

    explicit Client(QObject *parent = nullptr);
};
