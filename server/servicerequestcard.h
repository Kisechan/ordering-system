#pragma once
#ifndef SERVICEREQUESTCARD_H
#define SERVICEREQUESTCARD_H

#include <QWidget>
#include <QString>

// 服务请求状态枚举
enum class ServiceRequestStatus {
    Pending,      // 待完成
    Processing    // 正在处理
};

// 服务请求结构
struct ServiceRequest {
    int request_id;              // 请求ID
    QString customer_name;       // 客户名
    ServiceRequestStatus status; // 状态
    QString create_time;         // 创建时间

    ServiceRequest()
        : request_id(0)
        , status(ServiceRequestStatus::Pending)
    {}
};


class ElaText;
class ElaPushButton;

class ServiceRequestCard : public QWidget
{
    Q_OBJECT
public:
    explicit ServiceRequestCard(QWidget* parent = nullptr);

    void setRequest(const ServiceRequest& request);
    ServiceRequest getRequest() const { return m_request; }

signals:
    void processRequested(int requestId);   // 点击"处理"按钮 不需要与后端交互
    void completeRequested(int requestId);  // 点击"完成"按钮 不需要与厚度那交互

private:
    void refreshUI();

private:
    ServiceRequest m_request;

    ElaText* m_customerLabel = nullptr;// 名
    ElaText* m_statusLabel = nullptr;// 状态
    ElaText* m_timeLabel = nullptr;// 时间
    ElaPushButton* m_processBtn = nullptr;// 处理按钮
    ElaPushButton* m_completeBtn = nullptr;// 完成按钮
};

#endif // SERVICEREQUESTCARD_H
