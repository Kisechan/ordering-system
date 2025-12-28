#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class TcpClient : public QObject {
    Q_OBJECT

public:
    explicit TcpClient(QObject* parent = nullptr);

    // Connect to server
    void connectToServer(const QString& ip, quint16 port);

    // Send JSON data
    void sendJson(const QJsonObject& json);

signals:
    void connected();
    void disconnected();
    void jsonReceived(const QJsonObject& json);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    QTcpSocket* m_socket;
    QByteArray m_buffer; // Buffer for incoming data
};

#endif // TCPCLIENT_H