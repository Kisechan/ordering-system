#include <QApplication>
#include "ElaApplication.h"
#include "logindialog.h"
#include "clientmainwindow.h"

#include "client.h"
#include "NetworkManager.h"
#include "NetworkConfig.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 初始化网络连接
    Client* client = Client::getInstance();
    client->connectToServer(NetworkConfig::SERVER_IP, NetworkConfig::SERVER_PORT);
    
    // 初始化 NetworkManager
    NetworkManager* networkMgr = new NetworkManager(&a);
    networkMgr->connectToServer(NetworkConfig::SERVER_IP, NetworkConfig::SERVER_PORT);
    
    QObject::connect(&a, &QApplication::aboutToQuit, [](){
        Client::destroyInstance();
    });

    eApp->init();

    QFont f = qApp->font();
    f.setPixelSize(17);
    qApp->setFont(f);

    LoginDialog login(networkMgr);
    if (login.exec() == QDialog::Accepted) {
        auto* w = new ClientMainWindow(networkMgr, login.getUserId(), login.getUsername());
        w->show();
        return a.exec();
    }

    return 0;
}
