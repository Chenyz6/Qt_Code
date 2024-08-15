#-------------------------------------------------
#
# Project created by QtCreator 2018-09-06T19:36:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = project_frameworkV01
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


SOURCES += \
        main.cpp \
        widget.cpp \
    view/firstpagewidget.cpp \
    view/secondpagewidget.cpp \
    controller/firstpagecontroller.cpp \
    controller/secondpagecontroller.cpp \
    model/firstpagefunction.cpp \
    model/secondpagefunction.cpp

HEADERS += \
        widget.h \
    view/firstpagewidget.h \
    view/secondpagewidget.h \
    controller/firstpagecontroller.h \
    controller/secondpagecontroller.h \
    model/firstpagefunction.h \
    model/secondpagefunction.h

DISTFILES +=

RESOURCES += \
    res.qrc
