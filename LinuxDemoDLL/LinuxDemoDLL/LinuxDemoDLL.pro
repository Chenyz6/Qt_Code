QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    demodll.cpp

HEADERS += \
    demodll.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



# 移除默认的 gui 模块
QT -= gui

# 添加 core 和 gui 模块
QT += core gui

# 添加 concurrent 模块
Qt += concurren

# 指定这是一个库项目（而不是应用程序）
TEMPLATE = lib

# 定义一个宏 BDNAVIGATION_LIBRARY，可能用于条件编译。
DEFINES += DEMO_LIBRARY

# 启用 C++11 标准
CONFIG += c++11

# 添加编译器标志，确保使用 C++11 标准
QMAKE_CXXFLAGS += -std=c++11

# 指定生成的库文件的输出目录为 ./lib
DESTDIR = /home/ze/Desktop/Qt_Code/LinuxDemoDLL/LinuxDemoDLL/lib

# 可以添加依赖库
# LIBS += -L"../lib" -llibfftw3-3
# LIBS += -L"../lib" -llibfftw3f-3
# LIBS += -L"../lib" -llibfftw3l-3






