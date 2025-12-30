#pragma once
#ifndef SERVEDIALOG_H
#define SERVEDIALOG_H

#include "ElaContentDialog.h"
#include "servicerequest.h"

class ElaText;
class QVBoxLayout;

class ServeDialog : public ElaContentDialog
{
    Q_OBJECT
public:
    explicit ServeDialog(QWidget* parent = nullptr);

    void setTableInfo(const TableInfo& table);
    TableInfo getTableInfo() const { return m_table; }

signals:
    void dishServed(int tableNumber, const QString& dishName);

private slots:
    void onDishServed(const QString& dishName);

private:
    void rebuildDishList();

private:
    TableInfo m_table;

    ElaText* m_titleLabel = nullptr;
    QVBoxLayout* m_dishListLayout = nullptr;
    QWidget* m_dishListContainer = nullptr;
};

#endif // SERVEDIALOG_H
