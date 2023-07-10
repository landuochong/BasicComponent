/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_THREAD_H_
#define RTC_BASE_THREAD_H_

#include <stdint.h>

#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "absl/strings/string_view.h"
#include "event.h"

#if defined(WEBRTC_POSIX)
#include <pthread.h>
#endif

#if defined(WEBRTC_WIN)
#include <windows.h>
#endif

#include "absl/base/attributes.h"
#include "absl/functional/any_invocable.h"

#include "function_view.h"
#include "task_queue_base.h"
#include "thread_types.h"

#define RTC_LOG_THREAD_BLOCK_COUNT()
#define RTC_DCHECK_BLOCK_COUNT_NO_MORE_THAN(x)

namespace rtc {


class Thread : public webrtc::TaskQueueBase {
 public:
  static const int kForever = -1;

  Thread(bool do_init);

  // NOTE: ALL SUBCLASSES OF Thread MUST CALL Stop() IN THEIR DESTRUCTORS (or
  // guarantee Stop() is explicitly called before the subclass is destroyed).
  // This is required to avoid a data race between the destructor modifying the
  // vtable, and the Thread::PreRun calling the virtual method Run().

  // NOTE: SUBCLASSES OF Thread THAT OVERRIDE Clear MUST CALL
  // DoDestroy() IN THEIR DESTRUCTORS! This is required to avoid a data race
  // between the destructor modifying the vtable, and the ThreadManager
  // calling Clear on the object from a different thread.
  ~Thread() override;

  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  // Note: The behavior of Thread has changed.  When a thread is stopped,
  // futher Posts and Sends will fail.  However, any pending Sends and *ready*
  // Posts (as opposed to unexpired delayed Posts) will be delivered before
  // Get (or Peek) returns false.  By guaranteeing delivery of those messages,
  // we eliminate the race condition when an MessageHandler and Thread
  // may be destroyed independently of each other.
  virtual void Quit();
  virtual bool IsQuitting();
  virtual void Restart();
  // Not all message queues actually process messages (such as SignalThread).
  // In those cases, it's important to know, before posting, that it won't be
  // Processed.  Normally, this would be true until IsQuitting() is true.
  virtual bool IsProcessingMessagesForTesting();

  // Amount of time until the next message can be retrieved
  virtual int GetDelay();

  bool empty() const { return size() == 0u; }

  size_t size() const {
    MutexLock lock(&mutex_);
    return messages_.size() + delayed_messages_.size();
  }

  bool IsCurrent() const;

  // Sleeps the calling thread for the specified number of milliseconds, during
  // which time no processing is performed. Returns false if sleeping was
  // interrupted by a signal (POSIX only).
  static bool SleepMs(int millis);

  // Sets the thread's name, for debugging. Must be called before Start().
  // If `obj` is non-null, its value is appended to `name`.
  const std::string& name() const { return name_; }
  bool SetName(absl::string_view name, const void* obj);

  // Sets the expected processing time in ms. The thread will write
  // log messages when Dispatch() takes more time than this.
  // Default is 50 ms.
  void SetDispatchWarningMs(int deadline);

  // Starts the execution of the thread.
  bool Start();

  // Tells the thread to stop and waits until it is joined.
  // Never call Stop on the current thread.  Instead use the inherited Quit
  // function which will exit the base Thread without terminating the
  // underlying OS thread.
  virtual void Stop();

  // By default, Thread::Run() calls ProcessMessages(kForever).  To do other
  // work, override Run().  To receive and dispatch messages, call
  // ProcessMessages occasionally.
  virtual void Run();

  // Convenience method to invoke a functor on another thread.
  // Blocks the current thread until execution is complete.
  // Ex: thread.BlockingCall([&] { result = MyFunctionReturningBool(); });
  // NOTE: This function can only be called when synchronous calls are allowed.
  // See ScopedDisallowBlockingCalls for details.
  // NOTE: Blocking calls are DISCOURAGED, consider if what you're doing can
  // be achieved with PostTask() and callbacks instead.
  virtual void BlockingCall(FunctionView<void()> functor);

  template <typename Functor,
            typename ReturnT = std::invoke_result_t<Functor>,
            typename = typename std::enable_if_t<!std::is_void_v<ReturnT>>>
  ReturnT BlockingCall(Functor&& functor) {
    ReturnT result;
    BlockingCall([&] { result = std::forward<Functor>(functor)(); });
    return result;
  }

  // Allows BlockingCall to specified `thread`. Thread never will be
  // dereferenced and will be used only for reference-based comparison, so
  // instance can be safely deleted. If NDEBUG is defined and RTC_DCHECK_IS_ON
  // is undefined do nothing.
  void AllowInvokesToThread(Thread* thread);

  // If NDEBUG is defined and RTC_DCHECK_IS_ON is undefined do nothing.
  void DisallowAllInvokes();
  // Returns true if `target` was allowed by AllowInvokesToThread() or if no
  // calls were made to AllowInvokesToThread and DisallowAllInvokes. Otherwise
  // returns false.
  // If NDEBUG is defined and RTC_DCHECK_IS_ON is undefined always returns
  // true.
  bool IsInvokeToThreadAllowed(rtc::Thread* target);

  // From TaskQueueBase
  void Delete() override;
  void PostTask(absl::AnyInvocable<void() &&> task) override;
  void PostDelayedTask(absl::AnyInvocable<void() &&> task,
                       int64_t delay_us) override;
  void PostDelayedHighPrecisionTask(absl::AnyInvocable<void() &&> task,
                                    int64_t delay_us) override;

  // ProcessMessages will process I/O and dispatch messages until:
  //  1) cms milliseconds have elapsed (returns true)
  //  2) Stop() is called (returns false)
  bool ProcessMessages(int cms);

  // Returns true if this is a thread that we created using the standard
  // constructor, false if it was created by a call to
  // ThreadManager::WrapCurrentThread().  The main thread of an application
  // is generally not owned, since the OS representation of the thread
  // obviously exists before we can get to it.
  // You cannot call Start on non-owned threads.
  bool IsOwned();

  // Expose private method IsRunning() for tests.
  //
  // DANGER: this is a terrible public API.  Most callers that might want to
  // call this likely do not have enough control/knowledge of the Thread in
  // question to guarantee that the returned value remains true for the duration
  // of whatever code is conditionally executing because of the return value!
  bool RunningForTest() { return IsRunning(); }

  // These functions are public to avoid injecting test hooks. Don't call them
  // outside of tests.
  // This method should be called when thread is created using non standard
  // method, like derived implementation of rtc::Thread and it can not be
  // started by calling Start(). This will set started flag to true and
  // owned to false. This must be called from the current thread.
  bool WrapCurrent();
  void UnwrapCurrent();

 protected:
  // DelayedMessage goes into a priority queue, sorted by trigger time. Messages
  // with the same trigger time are processed in num_ (FIFO) order.
  struct DelayedMessage {
    bool operator<(const DelayedMessage& dmsg) const {
      return (dmsg.run_time_ms < run_time_ms) ||
             ((dmsg.run_time_ms == run_time_ms) &&
              (dmsg.message_number < message_number));
    }

    int64_t delay_ms;  // for debugging
    int64_t run_time_ms;
    // Monotonicaly incrementing number used for ordering of messages
    // targeted to execute at the same time.
    uint32_t message_number;
    // std::priority_queue doesn't allow to extract elements, but functor
    // is move-only and thus need to be changed when pulled out of the
    // priority queue. That is ok because `functor` doesn't affect operator<
    mutable absl::AnyInvocable<void() &&> functor;
  };

  // Perform initialization, subclasses must call this from their constructor
  // if false was passed as init_queue to the Thread constructor.
  void DoInit();

  // Perform cleanup; subclasses must call this from the destructor,
  // and are not expected to actually hold the lock.
  void DoDestroy() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  void WakeUp();

  // Same as WrapCurrent except that it never fails as it does not try to
  // acquire the synchronization access of the thread. The caller should never
  // call Stop() or Join() on this thread.
  void SafeWrapCurrent();

  // Blocks the calling thread until this thread has terminated.
  void Join();

 private:
  static const int kSlowDispatchLoggingThreshold = 50;  // 50 ms

  // Get() will process I/O until:
  //  1) A task is available (returns it)
  //  2) cmsWait seconds have elapsed (returns empty task)
  //  3) Stop() is called (returns empty task)
  absl::AnyInvocable<void() &&> Get(int cmsWait);
  void Dispatch(absl::AnyInvocable<void() &&> task);


#if defined(WEBRTC_WIN)
  static DWORD WINAPI PreRun(LPVOID context);
#else
  static void* PreRun(void* pv);
#endif

  // Return true if the thread is currently running.
  bool IsRunning();

  // Called by the ThreadManager when being set as the current thread.
  void EnsureIsCurrentTaskQueue();

  // Called by the ThreadManager when being unset as the current thread.
  void ClearCurrentTaskQueue();

  std::queue<absl::AnyInvocable<void() &&>> messages_ RTC_GUARDED_BY(mutex_);
  std::priority_queue<DelayedMessage> delayed_messages_ RTC_GUARDED_BY(mutex_);
  uint32_t delayed_next_num_ RTC_GUARDED_BY(mutex_);

  mutable absl::Mutex mutex_;
  bool fInitialized_;
  bool fDestroyed_;

  Event event_;

  std::atomic<int> stop_;
  std::string name_;

  // TODO(tommi): Add thread checks for proper use of control methods.
  // Ideally we should be able to just use PlatformThread.
#if defined(WEBRTC_POSIX)
  pthread_t thread_ = 0;
#endif
#if defined(WEBRTC_WIN)
  HANDLE thread_ = nullptr;
  DWORD thread_id_ = 0;
#endif

  basic_comm::PlatformThreadId cur_thread_id_ = 0;
  // Indicates whether or not ownership of the worker thread lies with
  // this instance or not. (i.e. owned_ == !wrapped).
  // Must only be modified when the worker thread is not running.
  bool owned_ = true;

  std::unique_ptr<TaskQueueBase::CurrentTaskQueueSetter> task_queue_registration_;
  int dispatch_warning_ms_ RTC_GUARDED_BY(this) = kSlowDispatchLoggingThreshold;
};


}  // namespace rtc

#endif  // RTC_BASE_THREAD_H_
