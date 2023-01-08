CONFIG   -= qt

TARGET = BasicComponent
TEMPLATE = lib
CONFIG += staticlib

DESTDIR = $$PWD/../build

DEFINES += QT_DEPRECATED_WARNINGS


CONFIG += c++11
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
    CONFIG += c++11
    QMAKE_LFLAGS_RELEASE += /MAP /DEBUG /opt:ref /INCREMENTAL:NO
}

INCLUDEPATH += $$PWD/../../src/
include(BasicComponent.pri)


contains(DEFINES, WIN64){
    LIBS += -L$$PWD/../third_party/openssl/openssl_lib_windows/x64/ -llibcrypto
    LIBS += -L$$PWD/../third_party/openssl/openssl_lib_windows/x64/ -llibssl
}else{
    LIBS += -L$$PWD/../third_party/openssl/openssl_lib_windows/x86/ -llibcrypto
    LIBS += -L$$PWD/../third_party/openssl/openssl_lib_windows/x86/ -llibssl
}

win32{
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
}