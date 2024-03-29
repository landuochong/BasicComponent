HEADERS += \
    $$PWD/crypt/curve25519.h \
    $$PWD/crypt/ibase64.h \
    $$PWD/crypt/pkcs7_padding.h \
    $$PWD/json/DataParseUtil.h \
    $$PWD/json/json.h \
    $$PWD/thread/ThreadPool.h \
    $$PWD/thread/event.h \
    $$PWD/thread/function_view.h \
    $$PWD/thread/task_queue_base.h \
    $$PWD/thread/thread.h \
    $$PWD/thread/thread_annotations.h \
    $$PWD/thread/thread_types.h \
    $$PWD/timer/TaskTimer.h \
    $$PWD/timer/Timer.h \
    $$PWD/utils/strutil.h \
    $$PWD/utils/time_utils.h

SOURCES += \
    $$PWD/crypt/ibase64.cc \
    $$PWD/crypt/pkcs7_padding.c \
    $$PWD/json/DataParseUtil.cc \
    $$PWD/thread/event.cc \
    $$PWD/thread/task_queue_base.cc \
    $$PWD/thread/thread.cc \
    $$PWD/thread/thread_types.cc \
    $$PWD/timer/TaskTimer.cc \
    $$PWD/timer/Timer.cpp \
    $$PWD/utils/strutil.cc \
    $$PWD/utils/time_utils.cc
