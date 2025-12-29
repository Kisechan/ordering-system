#include "orderinfo_page.h"
#include "ElaMessageBar.h"
#include "ElaText.h"
#include "ElaPushButton.h"
#include "ElaLineEdit.h"
#include "ElaText.h"
#include "ElaScrollArea.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

OrderInfo_Page::OrderInfo_Page(QWidget* parent):
    ElaScrollPage(parent)
{
    // ===== 页面不显示默认标题 =====
      setTitleVisible(false);

      // ===== 中央容器 =====
      center_widget_ = new QWidget(this);
      main_layout_   = new QVBoxLayout(center_widget_);
      main_layout_->setContentsMargins(0, 0, 20, 0);
      main_layout_->setSpacing(14);

      // ===== 顶部操作栏 =====
      QWidget* top_bar = new QWidget(center_widget_);
      QHBoxLayout* top_layout = new QHBoxLayout(top_bar);
      top_layout->setContentsMargins(0, 0, 0, 0);
      top_layout->setSpacing(10);

      ElaText* title = new ElaText("", top_bar);
      title->setTextPixelSize(0);

      user_search_edit_ = new ElaLineEdit(top_bar);
      user_search_edit_->setPlaceholderText("输入用户名查询");
      user_search_edit_->setFixedSize(180, 36);

      search_btn_ = new ElaPushButton("查询", top_bar);
      search_btn_->setFixedSize(80, 36);

      top_layout->addWidget(title);
      top_layout->addStretch();
      top_layout->addWidget(user_search_edit_);
      top_layout->addWidget(search_btn_);

      main_layout_->addWidget(top_bar);

      refresh_btn_ = new ElaPushButton("刷新", top_bar);
      refresh_btn_->setFixedSize(90, 36);

      top_layout->addWidget(title);
      top_layout->addStretch();
      top_layout->addWidget(refresh_btn_);

      main_layout_->addWidget(top_bar);

      //================= 表头行 =================
      QWidget* header = new QWidget(center_widget_);
      header->setFixedHeight(36);
      QHBoxLayout* header_layout = new QHBoxLayout(header);
      header_layout->setContentsMargins(12, 0, 12, 0);
      header_layout->setSpacing(20);

      ElaText* h_order  = new ElaText("订单号", header);
      ElaText* h_user   = new ElaText("用户名", header);
      ElaText* h_amount = new ElaText("总金额", header);
      ElaText* h_time   = new ElaText("下单时间", header);

      h_order->setFixedWidth(120);
      h_user->setFixedWidth(120);
      h_amount->setFixedWidth(100);

      header_layout->addWidget(h_order);
      header_layout->addWidget(h_user);
      header_layout->addWidget(h_amount);
      header_layout->addWidget(h_time);
      header_layout->addStretch();

      main_layout_->addWidget(header);

      // ================= 订单列表 =================
      QWidget* list_widget = new QWidget(center_widget_);
      order_list_layout_ = new QVBoxLayout(list_widget);
      order_list_layout_->setSpacing(6);
      order_list_layout_->setContentsMargins(0, 0, 0, 0);

      main_layout_->addWidget(list_widget);
      main_layout_->addStretch();

      addCentralWidget(center_widget_, true, true, 0);

      drawOrderList();

      // 查询
      connect(search_btn_, &ElaPushButton::clicked, this,&OrderInfo_Page::onSearch);

      // 回车触发查询（很加分）
      connect(user_search_edit_, &QLineEdit::returnPressed, this,&OrderInfo_Page::onSearch);

      // 刷新
      connect(refresh_btn_, &ElaPushButton::clicked, this, &OrderInfo_Page::onRefresh);

}

void OrderInfo_Page::onSearch()
{
    emit queryOrderByUser(user_search_edit_->text());
}

void OrderInfo_Page::onRefresh()
{
    emit refreshOrderRequested();
}


void OrderInfo_Page::drawOrderList()
{
    // 清空旧行
    QLayoutItem* item;
    while ((item = order_list_layout_->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    //======== 模拟订单行 ======
    for (int i = 0; i < 6; ++i) {
        QWidget* row = new QWidget(this);
        row->setFixedHeight(48);

        QHBoxLayout* layout = new QHBoxLayout(row);
        layout->setContentsMargins(12, 0, 12, 0);
        layout->setSpacing(20);

        ElaText* order_id = new ElaText(QString("#%1").arg(10000 + i), row);
        ElaText* user     = new ElaText("张三", row);
        ElaText* amount   = new ElaText("¥56.00", row);
        ElaText* time     = new ElaText("2025-12-29 11:20", row);

        order_id->setFixedWidth(120);
        user->setFixedWidth(120);
        amount->setFixedWidth(100);

        layout->addWidget(order_id);
        layout->addWidget(user);
        layout->addWidget(amount);
        layout->addWidget(time);
        layout->addStretch();

        order_list_layout_->addWidget(row);
    }
}


/*
void OrderInfo_Page::showNewOrderNotification()
{
    ElaMessageBar::success(ElaMessageBarType::BottomRight, "提示", "您有新的订单", 2000, this);
}
*/
OrderInfo_Page::~OrderInfo_Page() {

}
