#include "UserDao.h"
#include "DaoCommon.h"
#include <QDateTime>

namespace db {

ApiResponse<User> UserDao::login(const QString& username, const QString& password) const {
    ApiResponse<User> resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    if (!isAlphaNumOnly(password)) {
        resp.code = 402;
        resp.msg = "密码只能包含字母和数字！";
        return resp;
    }

    QSqlQuery q(db_);
    q.prepare("SELECT user_id, username, password, register_time FROM t_user WHERE username=? LIMIT 1");
    q.addBindValue(username);

    if (!q.exec()) { resp.code = 500; resp.msg = sqlErr(q); return resp; }
    if (!q.next()) { resp.code = 402; resp.msg = "该用户不存在！"; return resp; }

    const QString dbPass = q.value("password").toString();

    // ✅ 明文比较：输入与数据库保存相同即正确
    if (dbPass != password) {
        resp.code = 401;
        resp.msg = "密码错误！";
        return resp;
    }

    User u;
    u.setUserId(q.value("user_id").toInt());
    u.setUsername(q.value("username").toString());
    u.setPassword(dbPass);
    u.setRegisterTime(q.value("register_time").toDateTime());

    resp.code = 200;
    resp.msg = "登录成功";
    resp.data = u;
    return resp;
}

ApiResponseVoid UserDao::registerUser(const QString& username, const QString& password) const {
    ApiResponseVoid resp;

    QString err;
    if (!dbIsOpen(db_, &err)) { resp.code = 500; resp.msg = err; return resp; }

    if (!isAlphaNumOnly(password)) {
        resp.code = 402;
        resp.msg = "密码只能包含字母和数字！";
        return resp;
    }

    // check exists
    {
        QSqlQuery q(db_);
        q.prepare("SELECT user_id FROM t_user WHERE username=? LIMIT 1");
        q.addBindValue(username);
        if (!q.exec()) { resp.code = 500; resp.msg = sqlErr(q); return resp; }
        if (q.next()) { resp.code = 403; resp.msg = "用户名已存在！"; return resp; }
    }

    QSqlQuery q(db_);
    q.prepare("INSERT INTO t_user(username, password, register_time) VALUES(?, ?, ?)");
    q.addBindValue(username);
    q.addBindValue(password);
    q.addBindValue(QDateTime::currentDateTime());

    if (!q.exec()) { resp.code = 500; resp.msg = sqlErr(q); return resp; }

    resp.code = 200;
    resp.msg = "注册成功";
    return resp;
}

} // namespace db
