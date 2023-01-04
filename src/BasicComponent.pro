QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BasicComponent
TEMPLATE = app

DESTDIR = $$PWD/../build

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _CRT_SECURE_NO_WARNINGS

CONFIG += c++11
CONFIG+=force_debug_info
CONFIG+=separate_debug_info


SOURCES += \
        main.cpp

HEADERS += 

FORMS += 

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
