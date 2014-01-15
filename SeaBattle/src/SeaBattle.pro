#-------------------------------------------------
#
# Project created by QtCreator 2013-12-06T22:38:38
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SeaBattle
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    battlefield.cpp \
    gamemenu.cpp \
    shipsetupmenu.cpp \
    ship.cpp \
    logmenu.cpp \
    battlestat.cpp

HEADERS  += mainwindow.h \
    battlefield.h \
    gamemenu.h \
    shipsetupmenu.h \
    ship.h \
    common.h \
    logmenu.h \
    battlestat.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
