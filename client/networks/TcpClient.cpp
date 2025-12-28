#include "TcpClient.h"
#include "NetworkConfig.h"
#include <QJsonDocument>
#include <QDateTime>

TcpClient::TcpClient(QObject* parent)
    : QObject(parent), 
      m_socket(new QTcpSocket(this)),
      m_timeoutTimer(new QTimer(this)),
      m_isConnecting(false) {

    // 连接 QTcpSocket 信号到 TcpClient 槽函数
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, &TcpClient::onSocketError);

    // 连接超时定时器
    connect(m_timeoutTimer, &QTimer::timeout, this, &TcpClient::onConnectionTimeout);
    m_timeoutTimer->setSingleShot(true);
}

TcpClient::~TcpClient() {
    if (m_socket && m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

void TcpClient::connectToServer(const QString& ip, quint16 port, int timeoutMs) {
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        return; // 已连接
    }

    m_isConnecting = true;
    m_timeoutTimer->start(timeoutMs);
    m_socket->connectToHost(ip, port);
}

void TcpClient::sendJson(const QJsonObject& json) {
    if (m_socket->state() != QTcpSocket::ConnectedState) {
        emit connectionError("未连接到服务器");
        return;
    }

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    m_socket->write(data);
}

bool TcpClient::isConnected() const {
    return m_socket->state() == QTcpSocket::ConnectedState;
}

void TcpClient::disconnectFromServer() {
    m_timeoutTimer->stop();
    if (m_socket->state() != QTcpSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

void TcpClient::onReadyRead() {
    // 将接收到的数据追加到缓冲区
    m_buffer.append(m_socket->readAll());

    // 尝试解析完整的 JSON 对象
    while (!m_buffer.isEmpty()) {
        // 查找 JSON 对象的开始和结束
        int braceStart = m_buffer.indexOf('{');
        if (braceStart == -1) {
            m_buffer.clear();
            break;
        }

        // 移除开始前的数据
        if (braceStart > 0) {
            m_buffer.remove(0, braceStart);
        }

        // 计算括号匹配来找到 JSON 对象的结束
        int braceCount = 0;
        int endPos = -1;
        for (int i = 0; i < m_buffer.length(); ++i) {
            if (m_buffer[i] == '{') {
                braceCount++;
            } else if (m_buffer[i] == '}') {
                braceCount--;
                if (braceCount == 0) {
                    endPos = i;
                    break;
                }
            }
        }

        if (endPos == -1) {
            // 尚未接收到完整的 JSON 对象
            break;
        }

        // 提取完整的 JSON 对象
        QByteArray jsonData = m_buffer.left(endPos + 1);
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            // 发出 jsonReceived 信号
            emit jsonReceived(doc.object());
        }

        // 从缓冲区中移除已处理的 JSON 对象
        m_buffer.remove(0, endPos + 1);
    }
}

void TcpClient::onConnected() {
    m_isConnecting = false;
    m_timeoutTimer->stop();
    emit connected();
}

void TcpClient::onDisconnected() {
    m_isConnecting = false;
    m_timeoutTimer->stop();
    emit disconnected();
}

void TcpClient::onConnectionTimeout() {
    if (m_isConnecting) {
        m_socket->abort();
        emit connectionError("连接超时");
    }
}

void TcpClient::onSocketError() {
    QString errorMsg = m_socket->errorString();
    emit connectionError(errorMsg);
}