INCLUDEPATH += C:\opencv-4.9.0\Build\install\include
DEPENDPATH += C:\opencv-4.9.0\Build\install\include
Debug: {
LIBS += -LC:\opencv-4.9.0\Build\install\x64\vc16\lib -lopencv_world490d
}
Release: {
LIBS += -LC:\opencv-4.9.0\Build\install\x64\vc16\lib -lopencv_world490d
}
