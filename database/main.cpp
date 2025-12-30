#include <QCoreApplication>
#include <QDebug>

#include "db/DbManager.h"
#include "db/service/AuthService.h"
#include "db/service/DishService.h"
#include "db/service/ServiceService.h"
#include "db/service/OrderService.h"
#include "entity/DishCount.h"

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

void testOrder(QSqlDatabase conn) {
    qDebug() << "=====================================================================";
    qDebug() << "开始测试订单模块";

    if (!conn.isOpen()) {
        qCritical() << "数据库未打开";
        return;
    }

    db::OrderService orderService(conn);
    const int testUserId = 2;

    // ================== 1. 查询历史订单 ==================
    qDebug() << "\n[1] 查询历史订单";
    auto beforeResp = orderService.listOrdersByUser(testUserId);
    if (beforeResp["code"].toInt() != 200) {
        qCritical() << "listOrdersByUser failed:" << beforeResp;
        return;
    }

    QJsonArray beforeOrders = beforeResp["data"].toArray();
    int beforeCount = beforeOrders.size();
    qDebug() << "当前订单数量:" << beforeCount;

    for (const auto& oVal : beforeOrders) {
        QJsonObject o = oVal.toObject();
        qDebug() << "订单ID:" << o["order_id"].toInt()
                 << "总价:" << o["total_amount"].toDouble()
                 << "评价:" << o["comment"].toString()
                 << "时间:" << o["create_time"].toString();

        for (const auto& dVal : o["dishes"].toArray()) {
            QJsonObject d = dVal.toObject();
            QString rating = d["rating"].isNull()
                                 ? "未评分"
                                 : QString::number(d["rating"].toInt());
            qDebug() << "  菜品ID:" << d["dish_id"].toInt()
                     << "名称:" << d["name"].toString()
                     << "数量:" << d["count"].toInt()
                     << "价格:" << d["price"].toDouble()
                     << "评分:" << rating;
        }
    }

    // ================== 2. 提交新订单 ==================
    qDebug() << "\n[2] 提交新订单";
    QVector<db::DishCount> newOrder;
    newOrder.push_back({1, 2}); // 宫保鸡丁 x2
    newOrder.push_back({3, 1}); // 鱼香肉丝 x1

    auto submitResp = orderService.submitOrder(testUserId, newOrder);
    if (submitResp["code"].toInt() != 200) {
        qCritical() << "submitOrder failed:" << submitResp;
        return;
    }
    qDebug() << "submitOrder 成功";

    // ================== 3. 再次查询，验证数量变化 ==================
    qDebug() << "\n[3] 验证订单数量变化";
    auto afterResp = orderService.listOrdersByUser(testUserId);
    if (afterResp["code"].toInt() != 200) {
        qCritical() << "listOrdersByUser after submit failed:" << afterResp;
        return;
    }

    QJsonArray afterOrders = afterResp["data"].toArray();
    int afterCount = afterOrders.size();
    qDebug() << "提交后订单数量:" << afterCount
             << "变化:" << (afterCount - beforeCount);

    if (afterCount <= beforeCount) {
        qCritical() << "❌ 订单数量未增加，提交失败";
        return;
    }

    // ================== 4. 找到最新订单 ==================
    qDebug() << "\n[4] 查找最新订单";
    QJsonObject latestOrder;
    int maxOrderId = -1;

    for (const auto& oVal : afterOrders) {
        QJsonObject o = oVal.toObject();
        int oid = o["order_id"].toInt();
        if (oid > maxOrderId) {
            maxOrderId = oid;
            latestOrder = o;
        }
    }

    qDebug() << "最新订单ID:" << maxOrderId;

    // ================== 5. 提交评价 ==================
    qDebug() << "\n[5] 提交订单评价";
    QVector<int> dishIds;
    QVector<int> ratings;

    for (const auto& dVal : latestOrder["dishes"].toArray()) {
        QJsonObject d = dVal.toObject();
        dishIds.push_back(d["dish_id"].toInt());
        ratings.push_back(5); // 全部五星
    }

    auto commentResp = orderService.submitComment(
        maxOrderId,
        "测试评价：味道不错",
        dishIds,
        ratings
    );

    if (commentResp["code"].toInt() != 200) {
        qCritical() << "submitComment failed:" << commentResp;
        return;
    }
    qDebug() << "submitComment 成功";

    // ================== 6. 再次查询，验证评分 ==================
    qDebug() << "\n[6] 验证评分是否更新";
    auto verifyResp = orderService.listOrdersByUser(testUserId);
    if (verifyResp["code"].toInt() != 200) {
        qCritical() << "verify listOrdersByUser failed:" << verifyResp;
        return;
    }

    for (const auto& oVal : verifyResp["data"].toArray()) {
        QJsonObject o = oVal.toObject();
        if (o["order_id"].toInt() != maxOrderId) continue;

        for (const auto& dVal : o["dishes"].toArray()) {
            QJsonObject d = dVal.toObject();
            qDebug() << "菜品ID:" << d["dish_id"].toInt()
                     << "更新后评分:"
                     << (d["rating"].isNull()
                             ? "未评分"
                             : QString::number(d["rating"].toInt()));
        }
    }

    qDebug() << "\n订单模块测试完成";
    qDebug() << "=====================================================================";
}


int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    return runSmokeTest();
}
