#include "mainwindow.h"
#include <QApplication>

#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Client* client = Client::getInstance();
    client->connectToServer("127.0.0.1", 5523);
    QObject::connect(&a, &QApplication::aboutToQuit, [](){
        Client::destroyInstance();
    });

    MainWindow w;
    w.show();

    return a.exec();
}
