    QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
    py_func.cpp \
    widget.cpp \
    zuc128.c

HEADERS += \
    py_func.h \
    widget.h \
    zuc128.h

FORMS += \
    widget.ui

INCLUDEPATH += \
    E:\ffmpeg-5.1.2-full_build-shared\include \
    C:\Python310\include \
    C:\Python310\Lib\site-packages\numpy\core\include

# LIBS += -LE:/ffmpeg-5.1.2-full_build-shared/lib -lavcodec -lavdevice -lavfilter -lavformat -lpostproc -lavutil -lswscale



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



LIBS += -L$$PWD/../../../ffmpeg-5.1.2-full_build-shared/lib/ -lavcodec -lavdevice -lavfilter -lavformat -lpostproc
LIBS += -L$$PWD/../../../ffmpeg-5.1.2-full_build-shared/lib/ -lavutil
LIBS += -L$$PWD/../../../ffmpeg-5.1.2-full_build-shared/lib/ -lswscale



win32: LIBS += -LC:/Python310/libs/ -lpython3

INCLUDEPATH += C:/Python310/libs
DEPENDPATH += C:/Python310/libs

win32: LIBS += -LC:/Python310/libs/ -lpython310



LIBS += -L$$PWD/../../../opencv/opencv4.1.2/OpenCV-412-Contrib-MinGW64/x64/mingw/lib/ -lopencv_core412.dll

INCLUDEPATH += $$PWD/../../../opencv/opencv4.1.2/OpenCV-412-Contrib-MinGW64/include
