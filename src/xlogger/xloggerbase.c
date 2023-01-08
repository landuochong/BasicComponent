/*
 ============================================================================
 Name        : xloggerbase.c
 ============================================================================
 */

#include "comm/xlogger/xloggerbase.h"
#include <stdio.h>

#include "comm/compiler_util.h"

WEAK_FUNC  TLogLevel   __xlogger_Level_impl();
WEAK_FUNC  void        __xlogger_SetLevel_impl(TLogLevel _level);
WEAK_FUNC  int         __xlogger_IsEnabledFor_impl(TLogLevel _level);
WEAK_FUNC xlogger_appender_t __xlogger_SetAppender_impl(xlogger_appender_t _appender);
WEAK_FUNC void __xlogger_Write_impl(const XLoggerInfo* _info, const char* _log);
WEAK_FUNC void __xlogger_VPrint_impl(const XLoggerInfo* _info, const char* _format, va_list _list);

#ifndef WIN32
WEAK_FUNC const char* xlogger_dump(const void* _dumpbuffer, size_t _len);
WEAK_FUNC const char* xlogger_memory_dump(const void* _dumpbuffer, size_t _len);
#endif

TLogLevel   xlogger_Level() {
    if (NULL == &__xlogger_Level_impl)  return kLevelNone;
	return __xlogger_Level_impl();
}

void xlogger_SetLevel(TLogLevel _level){
    if (NULL != &__xlogger_SetLevel_impl)
        __xlogger_SetLevel_impl(_level);
}

int  xlogger_IsEnabledFor(TLogLevel _level) {
    if (NULL == &__xlogger_IsEnabledFor_impl) { return 0;}
	return __xlogger_IsEnabledFor_impl(_level);
}

xlogger_appender_t xlogger_SetAppender(xlogger_appender_t _appender) {
    if (NULL == &__xlogger_SetAppender_impl) { return NULL;}
    return __xlogger_SetAppender_impl(_appender);
}

static xlogger_filter_t sg_filter = NULL;
void xlogger_SetFilter(xlogger_filter_t _filter) {
    sg_filter = _filter;
}

xlogger_filter_t xlogger_GetFilter() {
    return sg_filter;
}

void xlogger_Write(const XLoggerInfo* _info, const char* _log) {
	if (NULL != &__xlogger_Write_impl)
		__xlogger_Write_impl(_info, _log);
}

void xlogger_VPrint(const XLoggerInfo* _info, const char* _format, va_list _list) {

}

void xlogger_Print(const XLoggerInfo* _info, const char* _format, ...) {

}


void xlogger_AssertP(const XLoggerInfo* _info, const char* _expression, const char* _format, ...) {

}

void xlogger_Assert(const XLoggerInfo* _info, const char* _expression, const char* _log) {

}

static TLogLevel gs_level = kLevelNone;
static xlogger_appender_t gs_appender = NULL;

TLogLevel   __xlogger_Level_impl() {return gs_level;}
void        __xlogger_SetLevel_impl(TLogLevel _level){ gs_level = _level;}
int         __xlogger_IsEnabledFor_impl(TLogLevel _level) {return gs_level <= _level;}

xlogger_appender_t __xlogger_SetAppender_impl(xlogger_appender_t _appender)  {
    xlogger_appender_t old_appender = gs_appender;
    gs_appender = _appender;
    return old_appender;
}

void __xlogger_Write_impl(const XLoggerInfo* _info, const char* _log) {

    if (!gs_appender) return;

    if (!__xlogger_IsEnabledFor_impl(__xlogger_Level_impl())) {
        return;
    }

    if (NULL == _log) {
        if (_info) {
            XLoggerInfo* info = (XLoggerInfo*)_info;
            info->level = kLevelFatal;
        }
        gs_appender(_info, "NULL == _log");
    } else {
        gs_appender(_info, _log);
    }
}

