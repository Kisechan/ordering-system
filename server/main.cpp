#include "mainwindow.h"
#include <QApplication>
#include "ElaApplication.h"


#include "server.h"

#include "DbManager.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    //Server* server = Server::getInstance();
    //server->start(5523);

    eApp->init();

    db::DbConfig cfg;
    cfg.odbcConnStr =
        "DRIVER={MySQL ODBC 9.1 Unicode Driver};"
        "TCPIP=1;"
        "SERVER=localhost;"
        "PORT=3306;"
        "DATABASE=restaurant;"
        "UID=root;"
        "PWD=123456;"
        "OPTION=3;";

    auto r = db::DbManager::instance().init(cfg);
    if (!r.isOk()) {
        qCritical() << "DB init failed:" << r.message;
    }

    MainWindow w;
    w.show();

    return a.exec();
}


