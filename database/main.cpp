#include <QCoreApplication>
#include <QDebug>

#include "db/DbManager.h"
#include "db/service/AuthService.h"
#include "db/service/DishService.h"
#include "db/service/ServiceService.h"
#include "db/service/OrderService.h"

#include <QRandomGenerator>

void testAuth(QSqlDatabase conn);
void testDish(QSqlDatabase conn);
void testOrder(QSqlDatabase conn);

static int runSmokeTest() {
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
    auto conn = db::DbManager::instance().db();

    //testAuth(conn);
    //testDish(conn);
    testOrder(conn);

    return 0;
}

void testAuth(QSqlDatabase conn) {
    qDebug() << "=====================================================================";
    qDebug() << "开始测试账号管理模块：";
    db::AuthService authService(conn);
    // 登录成功
    auto login = authService.login("user1", "user1123");
    if (login["code"].toInt() != 200) {
        qCritical() << "login failed:" << login;
    } else {
        qDebug() << "login ok:" << login;
    }

    // 用户名不存在
    login = authService.login("user114514", "user1123");
    if (login["code"].toInt() != 200) {
        qCritical() << "login failed:" << login;
    } else {
        qDebug() << "login ok:" << login;
    }

    // 密码错误
    login = authService.login("user1", "user1123575757");
    if (login["code"].toInt() != 200) {
        qCritical() << "login failed:" << login;
    } else {
        qDebug() << "login ok:" << login;
    }

    auto registerUser = authService.registerUser("user" + QString::number(QRandomGenerator::global()->bounded(100000, 999999)), "123456");
    if (registerUser["code"].toInt() != 200) {
        qCritical() << "registerUser failed:" << registerUser;
    } else {
        qDebug() << "registerUser ok:" << registerUser;
    }

    registerUser = authService.registerUser("user" + QString::number(QRandomGenerator::global()->bounded(100000, 999999)), "123456@@@");
    if (registerUser["code"].toInt() != 200) {
        qCritical() << "registerUser failed:" << registerUser;
    } else {
        qDebug() << "registerUser ok:" << registerUser;
    }
    qDebug() << "=====================================================================";
}

void testDish(QSqlDatabase conn) {
    qDebug() << "=====================================================================";
    qDebug() << "开始测试菜品模块：";
    db::DishService dishService(conn);
    auto dish = dishService.listAll();
    if (dish["code"].toInt() != 200) {
        qCritical() << "dish list failed:" << dish;
    } else {
        qDebug() << "dish list ok:" << dish;
    }
    qDebug() << "=====================================================================";
}

void testOrder(QSqlDatabase conn) {
    qDebug() << "=====================================================================";
    qDebug() << "开始测试订单模块：";
    db::OrderService orderService(conn);
    auto order = orderService.listOrdersByUser(2);
    if (order["code"].toInt() != 200) {
        qCritical() << "order list failed:" << order;
    } else {
        qDebug() << "order list ok:" << order;
    }
    qDebug() << "=====================================================================";
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    return runSmokeTest();
}
