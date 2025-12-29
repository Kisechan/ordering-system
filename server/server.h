#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QObject>
#include <QDebug>
#include <QSqlDatabase>

// 用户会话信息结构
struct UserSession {
    int userId;
    QString username;
};

class Server : public QTcpServer {
    Q_OBJECT
public:
    static Server* getInstance();

    // 启动服务端，绑定端口
    bool start(quint16 port);

signals:
    void callWaiter(QString userName);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void readData();

    void disconnected();

private:
    QMap<QTcpSocket*, UserSession> userMap;

    static Server* server;

    explicit Server(QObject* parent = nullptr);

    void processRequest(QTcpSocket* socket, const QJsonObject& request);

    void sendJson(QTcpSocket* socket, const QJsonObject& obj);
};

