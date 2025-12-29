#include "AuthService.h"
#include "../dao/UserDao.h"
#include "../dao/DaoCommon.h"
#include "../dao/JsonUtil.h"

namespace db {

QJsonObject AuthService::login(const QString& username, const QString& password) const {
    if (!isAlphaNumOnly(password)) return makeErr(402, "密码只能包含字母和数字！");

    UserDao dao(db_);
    auto r = dao.findByUsername(username);
    if (r.value("code").toInt() != 200) return r;

    const auto dv = r.value("data");
    if (dv.isNull() || !dv.isObject()) return makeErr(402, "该用户不存在！");

    const auto u = dv.toObject();
    if (u.value("password").toString() != password) return makeErr(401, "密码错误！");

    return makeOk(u, "登录成功");
}

QJsonObject AuthService::registerUser(const QString& username, const QString& password) const {
    if (!isAlphaNumOnly(password)) return makeErr(402, "密码只能包含字母和数字！");

    UserDao dao(db_);
    auto found = dao.findByUsername(username);
    if (found.value("code").toInt() != 200) return found;
    if (!found.value("data").isNull()) return makeErr(403, "用户名已存在！");

    auto ins = dao.insertUser(username, password);
    if (ins.value("code").toInt() != 200) return ins;

    return makeOk(QJsonValue::Null, "注册成功");
}

} // namespace db
