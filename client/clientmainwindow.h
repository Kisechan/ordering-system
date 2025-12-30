#pragma once


class HomePage;
class NetworkManager;
#include "cartmanager.h"
#include "ElaWindow.h"
#include <qtoolbutton.h>
#include <QContextMenuEvent>

class ClientMainWindow : public ElaWindow
{
    Q_OBJECT
public:
    explicit ClientMainWindow(NetworkManager* networkMgr = nullptr, QWidget* parent = nullptr);
    ~ClientMainWindow() override = default;
    
    NetworkManager* networkManager() const { return m_networkMgr; }

private:
    void hardRefresh();

protected:
    void contextMenuEvent(QContextMenuEvent* e) override;

private:
    CartManager* m_cart = nullptr;
    HomePage* m_home = nullptr;
    NetworkManager* m_networkMgr = nullptr;
};
