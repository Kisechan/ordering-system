#include "userservice.h"

#include <QCryptographicHash>

UserService* UserService::userService = new UserService();

UserService::UserService() {

}

UserService *UserService::getInstance() {
    return userService;
}

QJsonObject UserService::login(QString username, QString password) {
    QJsonObject response;
//    User user = UserMapper::getInstance().getByUserName(username);

//    if (!user.isValid()) {
//        response["code"] = 402;
//        response["msg"] = "用户不存在!";
//        return response;
//    }

//    QString hashedPassword = QCryptographicHash::hash(
//        password.toUtf8(),
//        QCryptographicHash::Sha256
//    ).toHex();
//    if (hashedPassword != user.getPassword()) {
//        response["code"] = 401;
//        response["msg"] = "密码错误！";
//        return response;
//    }

//    response["code"] = 200;
//    QJsonObject data;
//    data["user_id"] = user.getUserId();
//    response["data"] = data;
    return response;
}

QJsonObject UserService::registerAccount(QString username, QString password) {
    QJsonObject response;

//    User probUser = UserMapper::getInstance().getByUserName(username);
//    if (proUser.isValid()) {
//        response["code"] = 402;
//        response["msg"] = "该用户名已被占用！";
//        return response;
//    }



    return response;
}
