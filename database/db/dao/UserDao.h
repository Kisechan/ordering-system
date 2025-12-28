#pragma once
#include <QSqlDatabase>
#include <QString>
#include "ApiResponse.h"
#include "../entity/User.h"

namespace db {

class UserDao {
public:
    explicit UserDao(QSqlDatabase db): db_(std::move(db)) {}

    // design.md 2.1 用户登录
    // 200: 成功
    // 401: 密码错误
    // 402: 用户不存在 / 密码格式错误
    ApiResponse<User> login(const QString& username, const QString& password) const;

    // design.md 3.1 用户注册
    // 200: 注册成功
    // 402: 密码格式错误
    // 403: 用户已存在
    ApiResponseVoid registerUser(const QString& username, const QString& password) const;

private:
    QSqlDatabase db_;
};

} // namespace db
