#pragma once
#ifndef SERVICEREQUEST_PAGE_H
#define SERVICEREQUEST_PAGE_H

#include "ElaScrollPage.h"
#include "servicerequestcard.h"
#include <QList>

class ElaLineEdit;
class ElaPushButton;
class QVBoxLayout;
class QWidget;
class QTimer;

class ServiceRequest_Page : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit ServiceRequest_Page(QWidget* parent = nullptr);

    void setRequestList(const QList<ServiceRequest>& requests);

signals:
    void searchRequested(const QString& keyword);
    void refreshRequested();
    void requestProcessed(int requestId);   // 请求被标记为处理中
    void requestCompleted(int requestId);   // 请求被完成（移除）

private slots:
    void onSearchTextChanged(const QString& text);
    void onSearchReturnPressed();
    void onRefresh();
    void applyFilterNow();
    void onProcessRequest(int requestId);
    void onCompleteRequest(int requestId);

private:
    void rebuildList(const QList<ServiceRequest>& requests);

private:
    QWidget*        m_listContainer = nullptr;
    QVBoxLayout*    m_listLayout = nullptr;

    ElaLineEdit*    m_searchEdit = nullptr;
    ElaPushButton*  m_refreshBtn = nullptr;

    QTimer*         m_searchDebounce = nullptr;
    QList<ServiceRequest> m_allRequests;
    QString         m_keyword;
};
#endif // SERVICEREQUEST_PAGE_H
