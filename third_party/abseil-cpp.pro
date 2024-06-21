CONFIG   -= qt

TARGET = abseil-cpp
TEMPLATE = lib
CONFIG += staticlib

DESTDIR = $$PWD/../../build
DEFINES += QT_DEPRECATED_WARNINGS

#c++20
CONFIG += c++17

CONFIG+=force_debug_info
CONFIG+=separate_debug_info

win32{
    DEFINES += _WIN32
    DEFINES += WIN32
    DEFINES += _CRT_SECURE_NO_WARNINGS
    DEFINES += NOMINMAX
    DEFINES += WIN32_LEAN_AND_MEAN
    QMAKE_CFLAGS_RELEASE += -MT
    QMAKE_CXXFLAGS_RELEASE += -MT
    QMAKE_LFLAGS_RELEASE += /MAP /DEBUG /opt:ref /INCREMENTAL:NO
}


INCLUDEPATH += $$PWD/abseil-cpp/

include(abseil-cpp.pri)


