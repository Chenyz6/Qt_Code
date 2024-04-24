QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ChannelCoding/AwgnChannel.cpp \
    ChannelCoding/BpskModulate.cpp \
    ChannelCoding/DataRead_Write.cpp \
    ChannelCoding/LdpcDecode.cpp \
    ChannelCoding/LdpcEncode.cpp \
    ChannelCoding/Matrix.cpp \
    ChannelCoding/SignalGenerator.cpp \
    ChannelCoding/alloc.cpp \
    ChannelCoding/check.cpp \
    ChannelCoding/clsm_viterbi.cpp \
    ChannelCoding/conv_lsm_codec.cpp \
    ChannelCoding/dec.cpp \
    ChannelCoding/demo_viterbi.cpp \
    ChannelCoding/enc.cpp \
    ChannelCoding/intio.cpp \
    ChannelCoding/ldpc_codec.cpp \
    ChannelCoding/mod2convert.cpp \
    ChannelCoding/mod2dense.cpp \
    ChannelCoding/mod2sparse.cpp \
    ChannelCoding/nmcalc.cpp \
    ChannelCoding/open.cpp \
    ChannelCoding/rand.cpp \
    ChannelCoding/rc_bch.cpp \
    ChannelCoding/rc_rs.cpp \
    ChannelCoding/rcode.cpp \
    global.cpp \
    main.cpp \
    mainwindow.cpp \
    textedit.cpp

HEADERS += \
    ChannelCoding/AwgnChannel.h \
    ChannelCoding/BpskModulate.h \
    ChannelCoding/DataRead_Write.h \
    ChannelCoding/LdpcDecode.h \
    ChannelCoding/LdpcEncode.h \
    ChannelCoding/Matrix.h \
    ChannelCoding/SignalGenerator.h \
    ChannelCoding/alloc.h \
    ChannelCoding/check.h \
    ChannelCoding/clsm_viterbi.h \
    ChannelCoding/conv_lsm_codec.h \
    ChannelCoding/dec.h \
    ChannelCoding/enc.h \
    ChannelCoding/intio.h \
    ChannelCoding/ldpc_codec.h \
    ChannelCoding/mod2convert.h \
    ChannelCoding/mod2dense.h \
    ChannelCoding/mod2sparse.h \
    ChannelCoding/nmcalc.h \
    ChannelCoding/open.h \
    ChannelCoding/rand.h \
    ChannelCoding/rc_bch.h \
    ChannelCoding/rc_codec_base.h \
    ChannelCoding/rc_dict.h \
    ChannelCoding/rc_rs.h \
    ChannelCoding/rcode.h \
    Common_Inner.h \
    global.h \
    mainwindow.h \
    textedit.h

FORMS += \
    mainwindow.ui \
    textedit.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ChannelCoding/Result.txt \
    ChannelCoding/column_weight_distribution.txt \
    ChannelCoding/row_weight_distribution.txt

RESOURCES += \
    res.qrc
