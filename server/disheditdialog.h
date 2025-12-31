#pragma once
#ifndef DISHEDITDIALOG_H
#define DISHEDITDIALOG_H

#include "ElaContentDialog.h"
#include "dishcard.h"

class ElaLineEdit;
class ElaText;
class ElaPushButton;

class DishEditDialog : public ElaContentDialog
{
    Q_OBJECT
public:
    explicit DishEditDialog(QWidget* parent = nullptr);

    void setDish(const Dish& dish);
    Dish dish() const;
    void setAddMode(bool isAdd);  // 设置为新增模式

protected:
    void showEvent(QShowEvent* event) override;


private:
    Dish m_dish;
    bool m_isAddMode = false;  // 是否为新增模式

    ElaText*     m_titleLabel = nullptr;   // 对话框标题
    ElaLineEdit* m_nameEdit = nullptr;
    ElaLineEdit* m_priceEdit = nullptr;
    ElaLineEdit* m_categoryEdit = nullptr;
    ElaLineEdit* m_urlEdit = nullptr;
    ElaLineEdit* m_descEdit = nullptr;
    ElaText*     m_ratingLabel = nullptr;  // 评分只读显示
};

#endif // DISHEDITDIALOG_H
