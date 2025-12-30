#pragma once
#include <QDialog>
#include <QString>

class ElaPlainTextEdit;
class ElaPushButton;

class CommentDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CommentDialog(int orderId,
                           const QString& preset,
                           QWidget* parent = nullptr);

    QString comment() const;

private:
    int m_orderId = 0;
    ElaPlainTextEdit* m_edit = nullptr;
    ElaPushButton* m_ok = nullptr;
    ElaPushButton* m_cancel = nullptr;
};
