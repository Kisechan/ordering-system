QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11

INCLUDEPATH += $$PWD/ThirdParty/include

win32-g++:LIBS += -L$$PWD/ThirdParty/lib -lElaWidgetTools

win32: QMAKE_POST_LINK += $$quote(cmd /c copy /y $$shell_path($$PWD/ThirdParty/bin/ElaWidgetTools.dll) $$shell_path($$OUT_PWD))

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    logindialog.cpp \
    registerdialog.cpp \
    clientmainwindow.cpp \
    homepage.cpp \
    placeholderpage.cpp \
    dishcard.cpp \

HEADERS += \
    mainwindow.h \
    logindialog.h \
    registerdialog.h \
    clientmainwindow.h \
    homepage.h \
    placeholderpage.h \
    dishcard.h \

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
    ThirdParty/include/Image/Cirno.jpg \
    ThirdParty/include/Image/Moon.jpg \
    ThirdParty/include/Image/vvan.jpg \
    ThirdParty/include/Image/MicaBase.png \
    ThirdParty/include/Image/noise.png
