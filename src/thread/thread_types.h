#ifndef THREAD_TYPES_H_
#define THREAD_TYPES_H_

// clang-format off
// clang formating would change include order.
#if defined(WIN32) || defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#elif defined(ANDROID) || defined(__APPLE__)
#include <pthread.h>
#include <unistd.h>
#if defined(_MAC)
#include <pthread_spis.h>
#endif
#endif
// clang-format on

namespace lego_comm {
#if defined(WIN32)
typedef DWORD PlatformThreadId;
typedef DWORD PlatformThreadRef;
#elif defined(ANDROID) || defined(__APPLE__)
typedef pid_t PlatformThreadId;
typedef pthread_t PlatformThreadRef;
#endif

// Retrieve the ID of the current thread.
PlatformThreadId CurrentThreadId();

// Sets the current thread name.
void SetCurrentThreadName(const char* name);

}  // namespace rtc

#endif  // RTC_CORE_THREAD_TYPES_H_
