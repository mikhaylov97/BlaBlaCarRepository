#-------------------------------------------------
#
# Project created by QtCreator 2019-05-29T23:02:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlaBlaCar
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

CONFIG += c++11

SOURCES += \
        graphics_view_zoom.cpp \
        main.cpp \
        mainwindow.cpp \
        scene.cpp

HEADERS += \
    graphics_view_zoom.h \
        mainwindow.h \
    exceptions.h \
    scene.h \
    state.h \
    transition.h \
    allocator.h \
    iterator.h \
    statemachine.h \
    algorithm.h \
    serializer.h \
    car.h \
    passenger.h \
    manipulator.h


FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    graph.json


RESOURCES += \
    graph.json
