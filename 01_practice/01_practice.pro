
QT       += core gui
#高于4版本，添加 QT += widgets，为了兼容QT4
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = chenyz

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += \
    widget.h   # 头文件

FORMS += \
    widget.ui

TRANSLATIONS += \
    01_practice_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
