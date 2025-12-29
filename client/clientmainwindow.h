#pragma once

#include "ElaWindow.h"

class ClientMainWindow : public ElaWindow
{
    Q_OBJECT
public:
    explicit ClientMainWindow(QWidget* parent = nullptr);
    ~ClientMainWindow() override = default;
};
