#pragma once

#include <QWidget>

class PlaceholderPage : public QWidget
{
    Q_OBJECT
public:
    explicit PlaceholderPage(const QString& text, QWidget* parent = nullptr);
};
