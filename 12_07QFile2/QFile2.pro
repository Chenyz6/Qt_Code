QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += printsupport   #使用qcustomplot需要加此行

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Filecontrol.cpp \
    Global.cpp \
    main.cpp \
    mainwindow.cpp \
    mythread.cpp \
    qcustomplot.cpp

HEADERS += \
    Common_Inner.h \
    Common_Predef.h \
    Filecontrol.h \
    Global.h \
    mainwindow.h \
    mythread.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
