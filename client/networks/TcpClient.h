#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QTimer>

class TcpClient : public QObject {
    Q_OBJECT

public:
    explicit TcpClient(QObject* parent = nullptr);
    ~TcpClient();

    // Connect to server with timeout
    void connectToServer(const QString& ip, quint16 port, int timeoutMs = 5000);

    // Send JSON data
    void sendJson(const QJsonObject& json);

    // Get connection state
    bool isConnected() const;

    // Disconnect from server
    void disconnectFromServer();

signals:
    void connected();
    void disconnected();
    void jsonReceived(const QJsonObject& json);
    void connectionError(const QString& error); // 连接错误信号

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onConnectionTimeout();
    void onSocketError();

private:
    QTcpSocket* m_socket;
    QByteArray m_buffer;           // 接收缓冲区
    QTimer* m_timeoutTimer;        // 连接超时定时器
    bool m_isConnecting;           // 连接中标志
};

#endif // TCPCLIENT_H