#pragma once


class HomePage;
#include "cartmanager.h"
#include "ElaWindow.h"

class ClientMainWindow : public ElaWindow
{
    Q_OBJECT
public:
    explicit ClientMainWindow(QWidget* parent = nullptr);
    ~ClientMainWindow() override = default;

private:
    CartManager* m_cart = nullptr;
    HomePage* m_home = nullptr;
};
