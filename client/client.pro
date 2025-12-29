#-------------------------------------------------
#
# Project created by QtCreator 2025-12-28T09:54:45
#
#-------------------------------------------------

QT += core gui
QT += core gui network
QT += widgets


TARGET = client
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    client.cpp \
    Protocol.cpp \
    ResponseParser.cpp \
    NetworkManager.cpp \
    logindialog.cpp \
    registerdialog.cpp \
    clientmainwindow.cpp \
    homepage.cpp \
    placeholderpage.cpp \
    dishcard.cpp

HEADERS += \
    mainwindow.h \
    client.h \
    NetworkConfig.h \
    Protocol.h \
    ResponseParser.h \
    NetworkManager.h \
    logindialog.h \
    registerdialog.h \
    clientmainwindow.h \
    homepage.h \
    placeholderpage.h \
    dishcard.h

FORMS += \
    mainwindow.ui

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
