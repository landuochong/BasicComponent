#ifndef THREAD_TYPES_H_
#define THREAD_TYPES_H_

// clang-format off
// clang formating would change include order.
#if defined(WEBRTC_WIN)
#include <winsock2.h>
#include <windows.h>
#elif defined(WEBRTC_FUCHSIA)
#include <zircon/types.h>
#include <zircon/process.h>
#elif defined(WEBRTC_POSIX)
#include <pthread.h>
#include <unistd.h>
#if defined(WEBRTC_MAC)
#include <pthread_spis.h>
#endif
#endif
// clang-format on

namespace lego_comm {
#if defined(WEBRTC_WIN)
typedef DWORD PlatformThreadId;
typedef DWORD PlatformThreadRef;
#elif defined(WEBRTC_FUCHSIA)
typedef zx_handle_t PlatformThreadId;
typedef zx_handle_t PlatformThreadRef;
#elif defined(WEBRTC_POSIX)
typedef pid_t PlatformThreadId;
typedef pthread_t PlatformThreadRef;
#endif

// Retrieve the ID of the current thread.
PlatformThreadId CurrentThreadId();

// Sets the current thread name.
void SetCurrentThreadName(const char* name);

}  // namespace rtc

#endif  // RTC_CORE_THREAD_TYPES_H_
