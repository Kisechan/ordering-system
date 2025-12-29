# db/db.pri
# include from root .pro: include($$PWD/db/db.pri)
# $$PWD here is .../database/db

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/Result.h \
    $$PWD/DbManager.h \
    $$PWD/dao/DaoCommon.h \
    $$PWD/dao/JsonUtil.h \
    $$PWD/dao/UserDao.h \
    $$PWD/dao/DishDao.h \
    $$PWD/dao/OrderDao.h \
    $$PWD/dao/OrderDishDao.h \
    $$PWD/dao/OrderQueryDao.h \
    $$PWD/entity/User.h \
    $$PWD/entity/DishCount.h \
    $$PWD/service/AuthService.h \
    $$PWD/service/DishService.h \
    $$PWD/service/OrderService.h \
    $$PWD/service/ServiceService.h

SOURCES += \
    $$PWD/DbManager.cpp \
    $$PWD/dao/UserDao.cpp \
    $$PWD/dao/DishDao.cpp \
    $$PWD/dao/OrderDao.cpp \
    $$PWD/dao/OrderDishDao.cpp \
    $$PWD/dao/OrderQueryDao.cpp \
    $$PWD/service/AuthService.cpp \
    $$PWD/service/DishService.cpp \
    $$PWD/service/OrderService.cpp
