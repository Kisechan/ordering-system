#include "mainwindow.h"
#include <QApplication>
#include "ElaApplication.h"


#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Server* server = Server::getInstance();
    //server->start(5523);

    eApp->init();

    MainWindow w;
    w.show();

    return a.exec();
}


