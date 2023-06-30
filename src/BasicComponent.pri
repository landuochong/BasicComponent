HEADERS += \
    $$PWD/crypt/curve25519.h \
    $$PWD/crypt/ibase64.h \
    $$PWD/crypt/pkcs7_padding.h \
    $$PWD/json/DataParseUtil.h \
    $$PWD/json/json.h \
    $$PWD/thread/ThreadPool.h \
    $$PWD/thread/thread_types.h \
    $$PWD/timer/Timer.h

SOURCES += \
    $$PWD/crypt/ibase64.cc \
    $$PWD/crypt/pkcs7_padding.c \
    $$PWD/json/DataParseUtil.cc \
    $$PWD/thread/thread_types.cc
