#pragma once
#ifndef SERVICEREQUESTCARD_H
#define SERVICEREQUESTCARD_H

#include <QWidget>
#include <QString>
#include "servicerequest.h"


class ElaText;
class ElaPushButton;

class ServiceRequestCard : public QWidget
{
    Q_OBJECT
public:
    explicit ServiceRequestCard(QWidget* parent = nullptr);

    void setTableInfo(const TableInfo& table);
    TableInfo getTableInfo() const { return m_table; }

signals:
    void serveRequested(int tableNumber);      // 点击"上菜"按钮
    void completeRequested(int tableNumber);   // 点击"完成"按钮
    void handleCallRequested(int tableNumber); // 点击"处理呼叫"按钮

private:
    void refreshUI();

private:
    TableInfo m_table;

    ElaText* m_tableLabel = nullptr;
    ElaText* m_statusLabel = nullptr;
    ElaText* m_callLabel = nullptr;          // 呼叫提示
    ElaPushButton* m_serveBtn = nullptr;     // 上菜按钮
    ElaPushButton* m_completeBtn = nullptr;  // 完成按钮
    ElaPushButton* m_handleCallBtn = nullptr; // 处理呼叫按钮
};

#endif // SERVICEREQUESTCARD_H
