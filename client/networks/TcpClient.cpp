#include "TcpClient.h"
#include <QJsonDocument>
#include <QDateTime>

TcpClient::TcpClient(QObject* parent)
    : QObject(parent), m_socket(new QTcpSocket(this)) {

    // Connect QTcpSocket signals to TcpClient slots
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
}

void TcpClient::connectToServer(const QString& ip, quint16 port) {
    m_socket->connectToHost(ip, port);
}

void TcpClient::sendJson(const QJsonObject& json) {
    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    m_socket->write(data);
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
    emit connected();
}

void TcpClient::onDisconnected() {
    emit disconnected();
}