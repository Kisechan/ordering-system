#ifndef ORDERINFO_PAGE_H
#define ORDERINFO_PAGE_H
#include "ElaScrollPage.h"
#include "ordercard.h"
#include <QList>

class ElaLineEdit;
class ElaPushButton;
class QVBoxLayout;
class QWidget;
class QTimer;


class OrderInfo_Page : public ElaScrollPage {
    Q_OBJECT
public:
    Q_INVOKABLE explicit OrderInfo_Page(QWidget* parent = nullptr);
    ~OrderInfo_Page();

    // 设置订单列表
    void setOrderList(const QList<Order>& orders);

signals:
    // 用户点击"查询"
    void searchRequested(const QString& keyword);
    // 用户点击"刷新"
    void refreshOrderRequested();

private slots:
    void onSearchTextChanged(const QString& text);
    void onSearchReturnPressed();
    void onRefresh();
    void applyFilterNow();
    void onViewOrderDetail(int orderId);


private:
    void rebuildList(const QList<Order>& orders);

private:
    QWidget*        m_listContainer = nullptr;
    QVBoxLayout*    m_listLayout = nullptr;

    // 顶部查询区
    ElaLineEdit*    m_searchEdit = nullptr;
    ElaPushButton*  m_refreshBtn = nullptr;

    QTimer*         m_searchDebounce = nullptr;
    QList<Order>    m_allOrders;
    QString         m_keyword;

};


#endif // ORDERINFO_PAGE_H
