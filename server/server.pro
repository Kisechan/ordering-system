#-------------------------------------------------
#
# Project created by QtCreator 2025-12-28T09:54:04
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

# 1) 头文件路径
INCLUDEPATH += $$PWD/ThirdParty/include

# 2) 链接导入库（MinGW 用 .dll.a）
win32-g++:LIBS += $$PWD/ThirdParty/lib/libElaWidgetTools.dll.a

# 3) 运行时把 DLL 拷贝到 exe 同目录（Qt Creator shadow build 也能用）
win32: QMAKE_POST_LINK += $$quote(cmd /c copy /y $$shell_path($$PWD/ThirdParty/bin/ElaWidgetTools.dll) $$shell_path($$OUT_PWD))


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    server.cpp

HEADERS += \
    ThirdParty/include/Def.h \
    ThirdParty/include/ElaAcrylicUrlCard.h \
    ThirdParty/include/ElaAppBar.h \
    ThirdParty/include/ElaApplication.h \
    ThirdParty/include/ElaBreadcrumbBar.h \
    ThirdParty/include/ElaCalendar.h \
    ThirdParty/include/ElaCalendarPicker.h \
    ThirdParty/include/ElaCheckBox.h \
    ThirdParty/include/ElaColorDialog.h \
    ThirdParty/include/ElaComboBox.h \
    ThirdParty/include/ElaContentDialog.h \
    ThirdParty/include/ElaDockWidget.h \
    ThirdParty/include/ElaDoubleSpinBox.h \
    ThirdParty/include/ElaDxgiManager.h \
    ThirdParty/include/ElaEventBus.h \
    ThirdParty/include/ElaExponentialBlur.h \
    ThirdParty/include/ElaFlowLayout.h \
    ThirdParty/include/ElaGraphicsItem.h \
    ThirdParty/include/ElaGraphicsLineItem.h \
    ThirdParty/include/ElaGraphicsScene.h \
    ThirdParty/include/ElaGraphicsView.h \
    ThirdParty/include/ElaIcon.h \
    ThirdParty/include/ElaIconButton.h \
    ThirdParty/include/ElaImageCard.h \
    ThirdParty/include/ElaInteractiveCard.h \
    ThirdParty/include/ElaKeyBinder.h \
    ThirdParty/include/ElaLineEdit.h \
    ThirdParty/include/ElaListView.h \
    ThirdParty/include/ElaLog.h \
    ThirdParty/include/ElaMenu.h \
    ThirdParty/include/ElaMenuBar.h \
    ThirdParty/include/ElaMessageBar.h \
    ThirdParty/include/ElaMessageButton.h \
    ThirdParty/include/ElaMultiSelectComboBox.h \
    ThirdParty/include/ElaNavigationBar.h \
    ThirdParty/include/ElaNavigationRouter.h \
    ThirdParty/include/ElaPivot.h \
    ThirdParty/include/ElaPlainTextEdit.h \
    ThirdParty/include/ElaPopularCard.h \
    ThirdParty/include/ElaProgressBar.h \
    ThirdParty/include/ElaPromotionCard.h \
    ThirdParty/include/ElaPromotionView.h \
    ThirdParty/include/ElaPushButton.h \
    ThirdParty/include/ElaRadioButton.h \
    ThirdParty/include/ElaReminderCard.h \
    ThirdParty/include/ElaScrollArea.h \
    ThirdParty/include/ElaScrollBar.h \
    ThirdParty/include/ElaScrollPage.h \
    ThirdParty/include/ElaScrollPageArea.h \
    ThirdParty/include/ElaSlider.h \
    ThirdParty/include/ElaSpinBox.h \
    ThirdParty/include/ElaStatusBar.h \
    ThirdParty/include/ElaSuggestBox.h \
    ThirdParty/include/ElaTabBar.h \
    ThirdParty/include/ElaTabWidget.h \
    ThirdParty/include/ElaTableView.h \
    ThirdParty/include/ElaText.h \
    ThirdParty/include/ElaTheme.h \
    ThirdParty/include/ElaToggleButton.h \
    ThirdParty/include/ElaToggleSwitch.h \
    ThirdParty/include/ElaToolBar.h \
    ThirdParty/include/ElaToolButton.h \
    ThirdParty/include/ElaToolTip.h \
    ThirdParty/include/ElaTreeView.h \
    ThirdParty/include/ElaWidget.h \
    ThirdParty/include/ElaWindow.h \
    ThirdParty/include/singleton.h \
    ThirdParty/include/stdafx.h \
        mainwindow.h \
    server.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ThirdParty/include/ElaWidgetTools.qrc

DISTFILES += \
    ThirdParty/bin/ElaWidgetTools.dll \
    ThirdParty/include/Font/ElaAwesome.ttf \
    ThirdParty/include/Image/Cirno.jpg \
    ThirdParty/include/Image/MicaBase.png \
    ThirdParty/include/Image/Moon.jpg \
    ThirdParty/include/Image/noise.png \
    ThirdParty/lib/cmake/ElaWidgetToolsConfig.cmake \
    ThirdParty/lib/cmake/ElaWidgetToolsConfigVersion.cmake \
    ThirdParty/lib/libElaWidgetTools.dll.a
