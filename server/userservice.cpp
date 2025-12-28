#include "userservice.h"

#include <QCryptographicHash>

UserService* UserService::userService = nullptr;

UserService::UserService() {

}

UserService *UserService::getInstance() {
    if (userService == nullptr) {
        userService = new UserService();
    }
    return userService;
}

QJsonObject UserService::login(QString username, QString password) {
    QJsonObject response;
//    User user = userMapper.getByUserName(username);

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
