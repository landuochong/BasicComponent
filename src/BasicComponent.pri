HEADERS += \
    $$PWD/assert/__assert.h \
    $$PWD/compiler_util.h \
    $$PWD/container/RWLock.h \
    $$PWD/container/raii.h \
    $$PWD/container/threadsafe_queue.h \
    $$PWD/container/threadsafe_unordered_map.h \
    $$PWD/crypt/curve25519.h \
    $$PWD/crypt/ibase64.h \
    $$PWD/crypt/pkcs7_padding.h \
    $$PWD/json/DataParseUtil.h \
    $$PWD/json/json.h \
    $$PWD/thread/ThreadPool.h \
    $$PWD/thread/thread_types.h \
    $$PWD/timer/Timer.h

SOURCES += \
    $$PWD/assert/__assert.c \
    $$PWD/container/RWLock.cpp \
    $$PWD/crypt/ibase64.cc \
    $$PWD/crypt/pkcs7_padding.c \
    $$PWD/json/DataParseUtil.cc \
    $$PWD/thread/thread_types.cc
