#include <QApplication>
#include "ElaApplication.h"
#include "logindialog.h"
#include "clientmainwindow.h"

#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Client* client = Client::getInstance();
    client->connectToServer("127.0.0.1", 5523);
    QObject::connect(&a, &QApplication::aboutToQuit, [](){
        Client::destroyInstance();
    });

    eApp->init();

    QFont f = qApp->font();
    f.setPixelSize(17);
    qApp->setFont(f);

    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        auto* w = new ClientMainWindow();
        w->show();
        return a.exec();
    }

    return 0;
}
