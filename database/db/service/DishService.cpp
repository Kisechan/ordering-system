#include "DishService.h"
#include "../dao/DishDao.h"
#include "../dao/JsonUtil.h"

namespace db {

QJsonObject DishService::listAll() const {
    DishDao dao(db_);
    auto r = dao.listAll();
    if (r.value("code").toInt() == 200) {
        r["msg"] = "获取成功";
    }
    return r;
}

QJsonObject DishService::getById(int dishId) const {
    DishDao dao(db_);
    auto r = dao.findById(dishId);
    if (r.value("code").toInt() != 200) {
        return r;
    }
    if (r.value("data").isNull()) {
        return makeErr(404, "菜品不存在");
    }
    r["msg"] = "获取成功";
    return r;
}

} // namespace db
