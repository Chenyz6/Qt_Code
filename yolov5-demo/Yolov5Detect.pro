QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += exception
#CONFIG += console
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    yolov5.cpp

HEADERS += \
    mainwindow.h \
    yolov5.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

RC_ICONS = eye.ico
win32
{
#    message('运行win32版本')
#    INCLUDEPATH += E:\opencv452_Qt\install\include\
#                   E:\opencv452_Qt\install\include\opencv2
#    LIBS += -L E:\opencv452_Qt\install\x64\mingw\lib\libopencv_*.a

# #========================== opencv ==========================
#    INCLUDEPATH += D:\opencv_build\install\include\
#                   D:\opencv_build\install\include\opencv2

# #========================== opencv_gpu ==========================
#    INCLUDEPATH += D:\opencv455_cuda\install\include\
#                   D:\opencv455_cuda\install\include\opencv2

# #========================== openvino ==========================
#    INCLUDEPATH += "C:\Program Files (x86)\Intel\openvino_2021\opencv\include"
#    INCLUDEPATH += "C:\Program Files (x86)\Intel\openvino_2021\opencv\include\opencv2"
#    INCLUDEPATH += "C:\Program Files (x86)\Intel\openvino_2021\deployment_tools\inference_engine\include"

}


INCLUDEPATH +=  /usr/local/include/opencv4 \
            /usr/local/include/opencv4/opencv2

LIBS += /usr/local/lib/libopencv_calib3d.so \
        /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_core.so    \
        /usr/local/lib/libopencv_dnn.so    \
        /usr/local/lib/libopencv_features2d.so \
        /usr/local/lib/libopencv_flann.so \
        /usr/local/lib/libopencv_ml.so \
        /usr/local/lib/libopencv_imgproc.so \
        /usr/local/lib/libopencv_imgcodecs.so \
        /usr/local/lib/libopencv_objdetect.so \
        /usr/local/lib/libopencv_video.so \
        /usr/local/lib/libopencv_videoio.so \
        /usr/local/lib/libopencv_stitching.so \
        /usr/local/lib/libopencv_gapi.so \
        /usr/local/lib/libopencv_photo.so
