#include "disheditdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QWidget>
#include <QDebug>
#include <QShowEvent>
#include <QPushButton>

#include "ElaLineEdit.h"
#include "ElaText.h"
#include "ElaPushButton.h"

DishEditDialog::DishEditDialog(QWidget* parent)
    : ElaContentDialog(parent)
{
    setWindowTitle(QStringLiteral("编辑菜品"));
    setWindowModality(Qt::ApplicationModal);
    setMinimumSize(600, 500);

    auto* content = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 标题
    m_titleLabel = new ElaText(QStringLiteral("菜品信息编辑"), 20, content);
    m_titleLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(m_titleLabel);

    // 表单区域
    auto* formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // 菜品名称
    m_nameEdit = new ElaLineEdit(content);
    m_nameEdit->setPlaceholderText(QStringLiteral("请输入菜品名称"));
    m_nameEdit->setMinimumHeight(36);
    formLayout->addRow(QStringLiteral("菜品名称："), m_nameEdit);

    // 价格
    m_priceEdit = new ElaLineEdit(content);
    m_priceEdit->setPlaceholderText(QStringLiteral("请输入价格（元）"));
    m_priceEdit->setMinimumHeight(36);
    formLayout->addRow(QStringLiteral("价格："), m_priceEdit);

    // 品类
    m_categoryEdit = new ElaLineEdit(content);
    m_categoryEdit->setPlaceholderText(QStringLiteral("请输入品类（如：川菜、粤菜）"));
    m_categoryEdit->setMinimumHeight(36);
    formLayout->addRow(QStringLiteral("品类："), m_categoryEdit);

    // 评分（只读）
    m_ratingLabel = new ElaText("", 16, content);
    m_ratingLabel->setStyleSheet("color:#ff9500; font-weight: bold;");
    formLayout->addRow(QStringLiteral("评分："), m_ratingLabel);

    // 图片URL
    // ！！！注意：可优化功能：点击从用户本机寻找并添加到项目文件中
    m_urlEdit = new ElaLineEdit(content);
    m_urlEdit->setPlaceholderText(QStringLiteral("请输入图片URL（如：:/Image/dish.jpg）"));
    m_urlEdit->setMinimumHeight(36);
    formLayout->addRow(QStringLiteral("图片URL："), m_urlEdit);

    // 描述
    m_descEdit = new ElaLineEdit(content);
    m_descEdit->setPlaceholderText(QStringLiteral("请输入菜品描述"));
    m_descEdit->setMinimumHeight(36);
    formLayout->addRow(QStringLiteral("描述："), m_descEdit);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    // 底部按钮
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 10, 0, 0);
    buttonLayout->addStretch();

    auto* cancelBtn = new ElaPushButton(QStringLiteral("取消"), content);
    cancelBtn->setFixedSize(100, 36);
    QFont btnFont = cancelBtn->font();
    btnFont.setPixelSize(15);
    cancelBtn->setFont(btnFont);
    connect(cancelBtn, &ElaPushButton::clicked, this, &DishEditDialog::reject);

    auto* confirmBtn = new ElaPushButton(QStringLiteral("确认"), content);
    confirmBtn->setFixedSize(100, 36);
    confirmBtn->setFont(btnFont);
    connect(confirmBtn, &ElaPushButton::clicked, this, &DishEditDialog::accept);

    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(confirmBtn);

    mainLayout->addLayout(buttonLayout);

    setCentralWidget(content);
}

void DishEditDialog::setDish(const Dish& dish)
{
    m_dish = dish;

    if (m_nameEdit) {
        m_nameEdit->setText(m_dish.name);
    }
    if (m_priceEdit) {
        if (m_dish.price > 0) {
            m_priceEdit->setText(QString::number(m_dish.price, 'f', 2));
        } else {
            m_priceEdit->clear();
        }
    }
    if (m_categoryEdit) {
        m_categoryEdit->setText(m_dish.category);
    }
    if (m_ratingLabel) {
        if (m_isAddMode) {
            // 新增模式：显示默认未评分
            m_ratingLabel->setText(QStringLiteral("⭐ 未评分"));
        } else {
            // 编辑模式：显示当前评分且不可修改
            m_ratingLabel->setText(QStringLiteral("⭐ %1 ")
                                  .arg(QString::number(m_dish.rating, 'f', 1)));
        }
    }
    if (m_urlEdit) {
        m_urlEdit->setText(m_dish.url);
    }
    if (m_descEdit) {
        m_descEdit->setText(m_dish.description);
    }
}

void DishEditDialog::setAddMode(bool isAdd)
{
    m_isAddMode = isAdd;

    // 更新标题
    if (isAdd) {
        setWindowTitle(QStringLiteral("新增菜品"));
    } else {
        setWindowTitle(QStringLiteral("编辑菜品"));
    }
}

Dish DishEditDialog::dish() const
{
    Dish d = m_dish;  // 保留原有的 dish_id 和 rating

    if (m_nameEdit) {
        d.name = m_nameEdit->text().trimmed();
    }
    if (m_priceEdit) {
        bool ok;
        double price = m_priceEdit->text().toDouble(&ok);
        if (ok) d.price = price;
    }
    if (m_categoryEdit) {
        d.category = m_categoryEdit->text().trimmed();
    }
    if (m_urlEdit) {
        d.url = m_urlEdit->text().trimmed();
    }
    if (m_descEdit) {
        d.description = m_descEdit->text().trimmed();
    }

    return d;
}

void DishEditDialog::showEvent(QShowEvent* event)
{
    ElaContentDialog::showEvent(event);

    // 在对话框显示后，查找并隐藏底部的三个按钮
    QList<QPushButton*> buttons = this->findChildren<QPushButton*>();
    for (QPushButton* btn : buttons) {
        // 通过对象名或文本特征识别底部按钮并隐藏
        QString text = btn->text().toLower();
        if (text.contains("cancel") || text.contains("minimum") || text.contains("exit") ||
            text.contains("取消") || text.contains("最小化") || text.contains("退出")) {
            btn->hide();
        }
    }
}
