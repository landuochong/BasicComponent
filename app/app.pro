#-------------------------------------------------
#
# Project created by QtCreator 2021-08-03T10:33:47
#
#-------------------------------------------------

QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BasicComponentTest
TEMPLATE = app

DESTDIR = $$PWD/../../build

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _CRT_SECURE_NO_WARNINGS
DEFINES += WEBRTC_WIN

CONFIG += c++17
CONFIG+=force_debug_info
CONFIG+=separate_debug_info

win32{
    DEFINES += _WIN32
    DEFINES += WIN32
    DEFINES += WEBRTC_WIN
    DEFINES += _CRT_SECURE_NO_WARNINGS
    DEFINES += NOMINMAX
    DEFINES += WIN32_LEAN_AND_MEAN
    QMAKE_CFLAGS_RELEASE += -MT
    QMAKE_CXXFLAGS_RELEASE += -MT
    QMAKE_LFLAGS_RELEASE += /MAP /DEBUG /opt:ref /INCREMENTAL:NO
}


SOURCES += \
        main.cpp  \
        widget.cpp

HEADERS +=  \
    widget.h

FORMS +=  \
    widget.ui

INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../thread
INCLUDEPATH += $$PWD/../third_party/abseil-cpp/
INCLUDEPATH += $$PWD/../third_party/abseil-cpp/absl/

unix|win32: LIBS += -L$$DESTDIR/ -lBasicComponent
unix|win32: LIBS += -L$$DESTDIR/ -labseil-cpp

LIBS +=-lkernel32
LIBS +=-luser32
LIBS +=-lgdi32
LIBS +=-lwinspool
LIBS +=-lshell32
LIBS +=-lole32
LIBS +=-loleaut32
LIBS +=-luuid
LIBS +=-lcomdlg32
LIBS +=-ladvapi32
LIBS +=-lCrypt32
LIBS +=-lws2_32
LIBS +=-lcomsuppw



