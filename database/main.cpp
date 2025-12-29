#include <QCoreApplication>
#include <QDebug>

#include "db/DbManager.h"
#include "db/service/AuthService.h"
#include "db/service/DishService.h"
#include "db/service/ServiceService.h"

static int runSmokeTest() {
    db::DbConfig cfg;
    cfg.odbcConnStr =
        "DRIVER={MariaDB ODBC 3.2 Driver};"
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
        return 2;
    }

    auto conn = db::DbManager::instance().db();

    db::DishService dishSvc(conn);
    auto dishes = dishSvc.listAll();
    qDebug() << "dish count =" << dishes["data"].toArray().size();

    db::AuthService authSvc(conn);
    auto login = authSvc.login("user1", "user1123");
    if (login["code"].toInt() != 200) {
        qCritical() << "login failed:" << login;
        return 4;
    }
    qDebug() << "login ok:" << login["data"].toObject();

    db::ServiceService serviceSvc;
    qDebug() << serviceSvc.callWaiter("A01")["msg"].toString();

    qDebug() << "DB + JSON DAO SmokeTest OK.";
    return 0;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    return runSmokeTest();
}
