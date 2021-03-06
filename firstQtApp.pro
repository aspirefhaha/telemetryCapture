#-------------------------------------------------
#
# Project created by QtCreator 2018-11-20T18:25:47
#
#-------------------------------------------------

QT       += core gui widgets serialport opengl network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += charts

TARGET = firstQtApp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_COORD_TYPE=double

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    DBInConfWidget.cpp \
    TelemetryInWidget.cpp \
    ZMQInWidget.cpp \
    ZMQOutWidget.cpp \
    DBOutWidget.cpp \
    CenterWidget.cpp \
    callout.cpp \
    chartView.cpp \
    chartWidget.cpp \
    barwidget.cpp \
    Format3DS_OpenGL.cpp \
    Format3DS.cpp \
    GLWidget.cpp \
    Camera.cpp \
    commonutils.cpp

HEADERS += \
        mainwindow.h \
    DBInConfWidget.h \
    TelemetryInWidget.h \
    ZMQInWidget.h \
    ZMQOutWidget.h \
    DBOutWidget.h \
    CenterWidget.h \
    callout.h \
    chartView.h \
    chartWidget.h \
    barwidget.h \
    Format3DS.h \
    GLWidget.h \
    Camera.h \
    commonutils.h \
    telecommuproto.h

FORMS += \
        mainwindow.ui \
    dbinconf.ui \
    telemetryin.ui \
    zmqin.ui \
    zmqout.ui \
    dbout.ui \
    centerwindow.ui \
    chartwindow.ui \
    barwindow.ui

OBJECTS_DIR  = tmp
MOC_DIR      = tmp

# 根据平台不同而链接不同的库
unix {
LIBS += -lGLU -lGL -L /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/
}

win32 {
LIBS += -lglu32 -lopengl32
}

RESOURCES += \
    telemetrycapture.qrc
