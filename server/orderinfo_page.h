#ifndef ORDERINFO_PAGE_H
#define ORDERINFO_PAGE_H
#include "ElaScrollPage.h"
#include "ElaComboBox.h"
#include "ElaPushButton.h"

class ElaLineEdit;
class QVBoxLayout;
class QWidget;


class OrderInfo_Page : public ElaScrollPage {
    Q_OBJECT
public:
    Q_INVOKABLE explicit OrderInfo_Page(QWidget* parent = nullptr);
    ~OrderInfo_Page();

    // 刷新UI接口
    void drawOrderList();

signals:
    // 用户点击“查询”
    void queryOrderByUser(const QString& userName);

    // 用户点击“刷新”
    void refreshOrderRequested();

private slots:
    void onSearch();
    void onRefresh();


private:
    QWidget*        center_widget_{nullptr};
    QVBoxLayout*    main_layout_{nullptr};
    QVBoxLayout*    order_list_layout_{nullptr};

    // 顶部查询区
    ElaLineEdit*    user_search_edit_{nullptr};
    ElaPushButton* search_btn_{nullptr};
    ElaPushButton*  refresh_btn_{nullptr};
};


#endif // ORDERINFO_PAGE_H
