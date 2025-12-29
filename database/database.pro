QT += core sql
CONFIG += c++17 console
CONFIG -= app_bundle
TEMPLATE = app

include($$PWD/db/db.pri)

SOURCES += \
    $$PWD/main.cpp
