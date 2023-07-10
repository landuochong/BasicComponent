#ifndef RTC_BASE_EVENT_H_
#define RTC_BASE_EVENT_H_

#if defined(WEBRTC_WIN)
#include <windows.h>
#elif defined(WEBRTC_POSIX)
#include <pthread.h>
#else
#error "Must define either WEBRTC_WIN or WEBRTC_POSIX."
#endif

#include <stdint.h>
#include <cmath>
#include <limits>
#include <type_traits>
#include "thread_annotations.h"
#include "absl/synchronization/mutex.h"

namespace rtc {
#define RTC_DISALLOW_WAIT()

class RTC_SCOPED_LOCKABLE MutexLock final {
 public:
  MutexLock(const MutexLock&) = delete;
  MutexLock& operator=(const MutexLock&) = delete;

  explicit MutexLock(absl::Mutex* mutex) RTC_EXCLUSIVE_LOCK_FUNCTION(mutex)
      : mutex_(mutex) {
    mutex->Lock();
  }
  ~MutexLock() RTC_UNLOCK_FUNCTION() { mutex_->Unlock(); }

 private:
  absl::Mutex* mutex_;
};

class Event {
 public:
  // TODO(bugs.webrtc.org/14366): Consider removing this redundant alias.
  static constexpr int64_t kForever = std::numeric_limits<int64_t>::max();

  Event();
  Event(bool manual_reset, bool initially_signaled);
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  ~Event();

  void Set();
  void Reset();

  // Waits for the event to become signaled, but logs a warning if it takes more
  // than `warn_after`, and gives up completely if it takes more than
  // `give_up_after`. (If `warn_after >= give_up_after`, no warning will be
  // logged.) Either or both may be `kForever`, which means wait indefinitely.
  //
  // Care is taken so that the underlying OS wait call isn't requested to sleep
  // shorter than `give_up_after`.
  //
  // Returns true if the event was signaled, false if there was a timeout or
  // some other error.
  //give_up_after:us, warn_after:us
  bool Wait(int64_t give_up_after, int64_t warn_after);

  //如果无限等待，则3s提醒一次，否则不需中间提醒
  bool Wait(int64_t give_up_after) {
    return Wait(give_up_after, give_up_after == kForever
                                   ? 3*1'000'000
                                   : kForever);
  }

 private:
#if defined(WEBRTC_WIN)
  HANDLE event_handle_;
#elif defined(WEBRTC_POSIX)
  pthread_mutex_t event_mutex_;
  pthread_cond_t event_cond_;
  const bool is_manual_reset_;
  bool event_status_;
#endif
};



}  // namespace rtc

#endif  // RTC_BASE_EVENT_H_
