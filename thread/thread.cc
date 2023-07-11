/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "thread.h"

#if defined(WEBRTC_WIN)
#include <comdef.h>
#elif defined(WEBRTC_POSIX)
#include <time.h>
#else
#error "Either WEBRTC_WIN or WEBRTC_POSIX needs to be defined."
#endif

#if defined(WEBRTC_WIN)
// Disable warning that we don't care about:
// warning C4722: destructor never returns, potential memory leak
#pragma warning(disable : 4722)
#endif

#include <stdio.h>

#include <utility>

#include "absl/algorithm/container.h"
#include "absl/cleanup/cleanup.h"
#include "thread_types.h"
#include "time_utils.h"

#if defined(WEBRTC_MAC)
#include "rtc_base/system/cocoa_threading.h"

/*
 * These are forward-declarations for methods that are part of the
 * ObjC runtime. They are declared in the private header objc-internal.h.
 * These calls are what clang inserts when using @autoreleasepool in ObjC,
 * but here they are used directly in order to keep this file C++.
 * https://clang.llvm.org/docs/AutomaticReferenceCounting.html#runtime-support
 */
extern "C" {
void* objc_autoreleasePoolPush(void);
void objc_autoreleasePoolPop(void* pool);
}

namespace {
class ScopedAutoReleasePool {
 public:
  ScopedAutoReleasePool() : pool_(objc_autoreleasePoolPush()) {}
  ~ScopedAutoReleasePool() { objc_autoreleasePoolPop(pool_); }

 private:
  void* const pool_;
};
}  // namespace
#endif

namespace basic_comm {

Thread::Thread()
    : delayed_next_num_(0),
      fInitialized_(false),
      fDestroyed_(false),
      stop_(0){
  SetName("Thread", this);  // default name
  DoInit();
}


Thread::~Thread() {
  Stop();
  DoDestroy();
}

void Thread::DoInit() {
  if (fInitialized_) {
    return;
  }

  fInitialized_ = true;
}

void Thread::DoDestroy() {
  if (fDestroyed_) {
    return;
  }

  fDestroyed_ = true;
  // Clear.
  messages_ = {};
  delayed_messages_ = {};
}

void Thread::WakeUp() {
    event_.Set();
}

void Thread::Quit() {
  stop_.store(1, std::memory_order_release);
  WakeUp();
}

bool Thread::IsQuitting() {
  return stop_.load(std::memory_order_acquire) != 0;
}

void Thread::Restart() {
  stop_.store(0, std::memory_order_release);
}

absl::AnyInvocable<void() &&> Thread::Get(int cmsWait) {
  // Get w/wait + timer scan / dispatch + socket / event multiplexer dispatch

  int64_t cmsTotal = cmsWait;
  int64_t cmsElapsed = 0;
  int64_t msStart = TimeUtils::TimeMillis();
  int64_t msCurrent = msStart;
  while (true) {
    // Check for posted events
    int64_t cmsDelayNext = kForever;
    {
      // All queue operations need to be locked, but nothing else in this loop
      // can happen while holding the `mutex_`.
      MutexLock lock(&mutex_);
      // Check for delayed messages that have been triggered and calculate the
      // next trigger time.
      while (!delayed_messages_.empty()) {
        if (msCurrent < delayed_messages_.top().run_time_ms) {
          cmsDelayNext = delayed_messages_.top().run_time_ms - msCurrent;
          break;
        }
        messages_.push(std::move(delayed_messages_.top().functor));
        delayed_messages_.pop();
      }
      // Pull a message off the message queue, if available.
      if (!messages_.empty()) {
        absl::AnyInvocable<void()&&> task = std::move(messages_.front());
        messages_.pop();
        return task;
      }
    }

    if (IsQuitting())
      break;

    // Which is shorter, the delay wait or the asked wait?

    int64_t cmsNext;
    if (cmsWait == kForever) {
      cmsNext = cmsDelayNext;
    } else {
      cmsNext = std::max<int64_t>(0, cmsTotal - cmsElapsed);
      if ((cmsDelayNext != kForever) && (cmsDelayNext < cmsNext))
        cmsNext = cmsDelayNext;
    }

    {
      // Wait and multiplex in the meantime
      if (!event_.Wait(cmsNext == kForever ? Event::kForever
                                         : TimeUtils::TimeAfter(cmsNext),Event::kForever))
        return nullptr;
    }

    // If the specified timeout expired, return

    msCurrent = TimeUtils::TimeMillis();
    cmsElapsed = msCurrent - msStart;
    if (cmsWait != kForever) {
      if (cmsElapsed >= cmsWait)
        return nullptr;
    }
  }
  return nullptr;
}

void Thread::PostTask(absl::AnyInvocable<void() &&> task) {
  if (IsQuitting()) {
    return;
  }

  // Keep thread safe
  // Add the message to the end of the queue
  // Signal for the multiplexer to return
  {
    MutexLock lock(&mutex_);
    messages_.push(std::move(task));
  }
  WakeUp();
}

void Thread::PostDelayedHighPrecisionTask(absl::AnyInvocable<void() &&> task,
                                          int64_t delay_us) {
  if (IsQuitting()) {
    return;
  }

  // Keep thread safe
  // Add to the priority queue. Gets sorted soonest first.
  // Signal for the multiplexer to return.
  int64_t delay_ms = delay_us%1000>0?delay_us/1000+1:delay_us/1000;
  int64_t run_time_ms = TimeUtils::TimeAfter(delay_ms);
  {
    MutexLock lock(&mutex_);

    delayed_messages_.push({.delay_ms = delay_ms,
                            .run_time_ms = run_time_ms,
                            .message_number = delayed_next_num_,
                            .functor = std::move(task)
                           });
    // If this message queue processes 1 message every millisecond for 50 days,
    // we will wrap this number.  Even then, only messages with identical times
    // will be misordered, and then only briefly.  This is probably ok.
    ++delayed_next_num_;
  }
  WakeUp();
}

int Thread::GetDelay() {
  MutexLock lock(&mutex_);

  if (!messages_.empty())
    return 0;

  if (!delayed_messages_.empty()) {
    int delay = TimeUtils::TimeUntil(delayed_messages_.top().run_time_ms);
    if (delay < 0)
      delay = 0;
    return delay;
  }
  return kForever;
}

void Thread::Dispatch(absl::AnyInvocable<void() &&> task) {
  int64_t start_time = TimeUtils::TimeMillis();
  std::move(task)();
  int64_t end_time = TimeUtils::TimeMillis();
  int64_t diff = TimeUtils::TimeDiff(end_time, start_time);
  if (diff >= dispatch_warning_ms_) {
    // To avoid log spew, move the warning limit to only give warning
    // for delays that are larger than the one observed.
    dispatch_warning_ms_ = diff + 1;
  }
}


bool Thread::SleepMs(int milliseconds) {
#if defined(WEBRTC_WIN)
  ::Sleep(milliseconds);
  return true;
#else
  // POSIX has both a usleep() and a nanosleep(), but the former is deprecated,
  // so we use nanosleep() even though it has greater precision than necessary.
  struct timespec ts;
  ts.tv_sec = milliseconds / 1000;
  ts.tv_nsec = (milliseconds % 1000) * 1000000;
  int ret = nanosleep(&ts, nullptr);
  if (ret != 0) {
    RTC_LOG_ERR(LS_WARNING) << "nanosleep() returning early";
    return false;
  }
  return true;
#endif
}

bool Thread::SetName(std::string name, const void* obj) {
  name_ = name;
  if (obj) {
    // The %p specifier typically produce at most 16 hex digits, possibly with a
    // 0x prefix. But format is implementation defined, so add some margin.
    char buf[30];
    snprintf(buf, sizeof(buf), " 0x%p", obj);
    name_ += buf;
  }
  return true;
}

void Thread::SetDispatchWarningMs(int deadline) {
  if (!IsCurrent()) {
    PostTask([this, deadline]() { SetDispatchWarningMs(deadline); });
    return;
  }
  dispatch_warning_ms_ = deadline;
}

bool Thread::Start() {
  if (IsRunning())
    return false;

  Restart();  // reset IsQuitting() if the thread is being restarted

  owned_ = true;

#if defined(WEBRTC_WIN)
  thread_ = CreateThread(nullptr, 0, PreRun, this, 0, &thread_id_);
  if (!thread_) {
    return false;
  }
#elif defined(WEBRTC_POSIX)
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  int error_code = pthread_create(&thread_, &attr, PreRun, this);
  if (0 != error_code) {
    RTC_LOG(LS_ERROR) << "Unable to create pthread, error " << error_code;
    thread_ = 0;
    return false;
  }
  RTC_DCHECK(thread_);
#endif
  return true;
}


void Thread::Join() {
  if (!IsRunning())
    return;

#if defined(WEBRTC_WIN)
  WaitForSingleObject(thread_, INFINITE);
  CloseHandle(thread_);
  thread_ = nullptr;
  thread_id_ = 0;
#elif defined(WEBRTC_POSIX)
  pthread_join(thread_, nullptr);
  thread_ = 0;
#endif
}

// static
#if defined(WEBRTC_WIN)
DWORD WINAPI Thread::PreRun(LPVOID pv) {
#else
void* Thread::PreRun(void* pv) {
#endif

  Thread* thread = static_cast<Thread*>(pv);
  basic_comm::SetCurrentThreadName(thread->name_.c_str());
  thread->cur_thread_id_ = basic_comm::CurrentThreadId();

#if defined(WEBRTC_MAC)
  ScopedAutoReleasePool pool;
#endif

  thread->Run();

#ifdef WEBRTC_WIN
  return 0;
#else
  return nullptr;
#endif
}  // namespace rtc

void Thread::Run() {
  ProcessMessages(kForever);
}

bool Thread::IsOwned() {
  return owned_;
}

void Thread::Stop() {
  Thread::Quit();
  Join();
}

bool Thread::IsCurrent() const {
  return CurrentThreadId() == cur_thread_id_;
}

void Thread::BlockingCall(FunctionView<void()> functor) {
  if (IsQuitting())
    return;

  if (IsCurrent()) {//判断是当前线程执行
    functor();
    return;
  }

  // Perhaps down the line we can get rid of this workaround and always require
  // current_thread to be valid when BlockingCall() is called.
  std::unique_ptr<Event> done_event;
  done_event.reset(new Event());

  absl::Cleanup cleanup = [this, done = done_event.get()] {
      done->Set();
  };
  //添加任务
  PostTask([functor, cleanup = std::move(cleanup)] { functor(); });
  done_event->Wait(Event::kForever);
}

// Called by the ThreadManager when being set as the current thread.
void Thread::EnsureIsCurrentTaskQueue() {
  task_queue_registration_ =
      std::make_unique<TaskQueueBase::CurrentTaskQueueSetter>(this);
}

// Called by the ThreadManager when being set as the current thread.
void Thread::ClearCurrentTaskQueue() {
  task_queue_registration_.reset();
}

void Thread::Delete() {
  Stop();
  delete this;
}

void Thread::PostDelayedTask(absl::AnyInvocable<void() &&> task,
                             int64_t delay_us) {
  // This implementation does not support low precision yet.
  PostDelayedHighPrecisionTask(std::move(task), delay_us);
}

bool Thread::IsProcessingMessagesForTesting() {
  return (owned_ || IsCurrent()) && !IsQuitting();
}

//cmsLoop:ms
bool Thread::ProcessMessages(int cmsLoop) {
  int64_t msEnd = (kForever == cmsLoop) ? 0 : TimeUtils::TimeAfter(cmsLoop);
  int cmsNext = cmsLoop;

  while (true) {
#if defined(WEBRTC_MAC)
    ScopedAutoReleasePool pool;
#endif
    absl::AnyInvocable<void()&&> task = Get(cmsNext);
    if (!task)
      return !IsQuitting();
    Dispatch(std::move(task));

    if (cmsLoop != kForever) {
      cmsNext = static_cast<int>(TimeUtils::TimeUntil(msEnd));
      if (cmsNext < 0)
        return true;
    }
  }
}

bool Thread::IsRunning() {
#if defined(WEBRTC_WIN)
  return thread_ != nullptr;
#elif defined(WEBRTC_POSIX)
  return thread_ != 0;
#endif
}

}  // namespace rtc
