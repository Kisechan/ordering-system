#include <QCoreApplication>
#include <QDebug>

#include "db/DbManager.h"
#include "db/dao/UserDao.h"
#include "db/dao/DishDao.h"
#include "db/dao/OrderDao.h"
#include "db/dao/ServiceDao.h"

static int runSmokeTest() {
    // 1) DB connect
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
    db::UserDao userDao(conn);
    db::DishDao dishDao(conn);
    db::OrderDao orderDao(conn);

    // 2) Dish list
    auto dishesResp = dishDao.listAll();
    if (!dishesResp.ok()) {
        qCritical() << "Dish list failed:" << dishesResp.code << dishesResp.msg;
        return 3;
    }
    qDebug() << "dish count =" << dishesResp.data.size();

    // 3) Login (restaurant.sql default user: user1 / user1123)
    auto login = userDao.login("user1", "user1123");
    if (!login.ok()) {
        qCritical() << "login failed:" << login.code << login.msg;
        return 4;
    }
    qDebug() << "login ok, user_id=" << login.data.userId() << "username=" << login.data.username();

    // 4) Call waiter (stub)
    db::ServiceDao serviceDao;
    auto call = serviceDao.callWaiter("A01");
    if (call.ok()) qDebug() << call.msg;

    // 5) Optional: submit an order (uncomment if you want to test transaction)
    /*
    QVector<db::DishCount> cart;
    cart.push_back(db::DishCount(1, 2));
    cart.push_back(db::DishCount(3, 1));

    auto submit = orderDao.submitOrder(login.data.userId(), cart);
    if (!submit.ok()) {
        qCritical() << "submitOrder failed:" << submit.code << submit.msg;
        return 5;
    }
    qDebug() << submit.msg;

    auto history = orderDao.listOrdersByUser(login.data.userId());
    if (!history.ok()) {
        qCritical() << "order history failed:" << history.code << history.msg;
        return 6;
    }
    qDebug() << "order history size =" << history.data.size();
    */

    qDebug() << "DB + DAO SmokeTest OK.";
    return 0;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    return runSmokeTest();
}
