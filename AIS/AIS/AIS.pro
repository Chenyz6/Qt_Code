QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AIS.cpp \
    DataFIRDF.cpp \
    DataRead_Write.cpp \
    SgnlPrcsDll.cpp \
    SignalDemodProbe.cpp \
    SignalParamProbe.cpp \
    SpectrumProbe.cpp \
    acars.cpp \
    basicFunc.cpp \
    main.cpp \
    mainwindow.cpp \
    stdafx.cpp

HEADERS += \
    ACARS.h \
    AIS.h \
    CmnHdr.h \
    DataChannelize.h \
    DataFIRDF.h \
    DataRead_Write.h \
    OfflineError.h \
    SgnlPrcsDll.h \
    SignalDemodFIR.h \
    SignalDemodProbe.h \
    SignalParamProbe.h \
    SignalProcessing.h \
    SpectrumProbe.h \
    atlstr.h \
    basicFunc.h \
    fdacoefs.h \
    fftw3.h \
    lowpass.h \
    mainwindow.h \
    protocolUnpackage.h \
    stdafx.h \
    targetver.h \
    tmwtypes.h \
    type.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/./ -lfftw3

INCLUDEPATH += $$PWD/''
DEPENDPATH += $$PWD/''
