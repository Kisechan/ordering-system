# db/db.pri
# 注意：$$PWD 本身就是 .../database/db

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/Result.h \
    $$PWD/DbManager.h \
    $$PWD/dao/ApiResponse.h \
    $$PWD/dao/DaoCommon.h \
    $$PWD/dao/UserDao.h \
    $$PWD/dao/DishDao.h \
    $$PWD/dao/OrderDao.h \
    $$PWD/dao/ServiceDao.h \
    $$PWD/entity/User.h \
    $$PWD/entity/Dish.h \
    $$PWD/entity/DishCount.h \
    $$PWD/entity/OrderDishItem.h \
    $$PWD/entity/Order.h \
    $$PWD/entity/AdminOrderBrief.h

SOURCES += \
    $$PWD/DbManager.cpp \
    $$PWD/dao/UserDao.cpp \
    $$PWD/dao/DishDao.cpp \
    $$PWD/dao/OrderDao.cpp
