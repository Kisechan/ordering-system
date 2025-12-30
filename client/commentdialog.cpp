#include "CommentDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "ElaText.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"

CommentDialog::CommentDialog(int orderId, const QString& preset, QWidget* parent)
    : QDialog(parent), m_orderId(orderId)
{
    setWindowTitle(QStringLiteral("编辑评论"));
    setModal(true);
    setFixedSize(420, 260);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 14, 16, 14);
    root->setSpacing(10);

    auto* title = new ElaText(QStringLiteral("订单 #%1").arg(orderId), 16, this);
    root->addWidget(title);

    m_edit = new ElaPlainTextEdit(this);
    m_edit->setPlaceholderText(QStringLiteral("请输入评论（可留空）"));
    m_edit->setPlainText(preset);
    root->addWidget(m_edit, 1);

    auto* row = new QHBoxLayout();
    row->addStretch(1);

    m_cancel = new ElaPushButton(QStringLiteral("取消"), this);
    m_ok     = new ElaPushButton(QStringLiteral("保存"), this);
    m_cancel->setMinimumHeight(32);
    m_ok->setMinimumHeight(32);

    row->addWidget(m_cancel);
    row->addWidget(m_ok);
    root->addLayout(row);

    connect(m_cancel, &ElaPushButton::clicked, this, &QDialog::reject);
    connect(m_ok, &ElaPushButton::clicked, this, &QDialog::accept);
}

QString CommentDialog::comment() const
{
    return m_edit ? m_edit->toPlainText().trimmed() : QString();
}
