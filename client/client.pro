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

INCLUDEPATH += $$PWD/ThirdParty/include

win32-g++:LIBS += $$PWD/ThirdParty/lib/libElaWidgetTools.dll.a

win32: QMAKE_POST_LINK += $$quote(cmd /c copy /y $$shell_path($$PWD/ThirdParty/bin/ElaWidgetTools.dll) $$shell_path($$OUT_PWD))

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    client.cpp \
    clientmainwindow.cpp \
    dishcard.cpp \
    homepage.cpp \
    logindialog.cpp \
    NetworkManager.cpp \
    placeholderpage.cpp \
    Protocol.cpp \
    registerdialog.cpp \
    ResponseParser.cpp \
    cartmanager.cpp \
    cartdishcard.cpp \
    cartpage.cpp \
    orderhistorypage.cpp \
    ordercard.cpp \
    commentdialog.cpp \
    ratedishcard.cpp \
    ratedialog.cpp \
    accountpage.cpp \
    reviewdialog.cpp

HEADERS += \
        mainwindow.h \
    client.h \
    clientmainwindow.h \
    dishcard.h \
    homepage.h \
    logindialog.h \
    NetworkConfig.h \
    NetworkManager.h \
    placeholderpage.h \
    Protocol.h \
    registerdialog.h \
    ResponseParser.h \
    cartmanager.h \
    carttype.h \
    cartdishcard.h \
    cartpage.h \
    orderhistorypage.h \
    ordercard.h \
    commentdialog.h \
    ratedishcard.h \
    ratedialog.h \
    accountpage.h \
    reviewdialog.h

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

DISTFILES += \
    ThirdParty/include/Font/ElaAwesome.ttf \
    ThirdParty/bin/ElaWidgetTools.dll \
    ThirdParty/include/Image/Cirno.jpg \
    ThirdParty/include/Image/Moon.jpg \
    ThirdParty/include/Image/vvan.jpg \
    ThirdParty/include/Image/MicaBase.png \
    ThirdParty/include/Image/noise.png
