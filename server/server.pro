#-------------------------------------------------
#
# Project created by QtCreator 2025-12-28T09:54:04
#
#-------------------------------------------------

QT       += core gui
QT       += network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler).
# Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

INCLUDEPATH += $$PWD/ThirdParty/include

win32-g++:LIBS += $$PWD/ThirdParty/lib/libElaWidgetTools.dll.a

win32: QMAKE_POST_LINK += $$quote(cmd /c copy /y $$shell_path($$PWD/ThirdParty/bin/ElaWidgetTools.dll) $$shell_path($$OUT_PWD))


SOURCES += \
        dishcard.cpp \
        disheditdialog.cpp \
        dishinordercard.cpp \
        dishmanage_page.cpp \
        main.cpp \
        mainwindow.cpp \
        ordercard.cpp \
        orderdetaildialog.cpp \
        orderinfo_page.cpp \
        servedialog.cpp \
        server.cpp \
        servicerequest_page.cpp \
        servicerequestcard.cpp

HEADERS += \
        dishcard.h \
        disheditdialog.h \
        dishinordercard.h \
        dishmanage_page.h \
        mainwindow.h \
        ordercard.h \
        orderdetaildialog.h \
        orderinfo_page.h \
        servedialog.h \
        server.h \
        servicerequest.h \
        servicerequest_page.h \
        servicerequestcard.h

FORMS += \
    mainwindow.ui

# 引入 database 模块的配置
include(../database/db/db.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ThirdParty/include/ElaWidgetTools.qrc

OTHER_FILES += \
    $$files($$PWD/ThirdParty/include/*.h, true) \
    $$PWD/ThirdParty/lib/libElaWidgetTools.dll.a \
    $$PWD/ThirdParty/bin/ElaWidgetTools.dll \
    $$PWD/ThirdParty/lib/cmake/ElaWidgetToolsConfig.cmake \
    $$PWD/ThirdParty/lib/cmake/ElaWidgetToolsConfigVersion.cmake

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
