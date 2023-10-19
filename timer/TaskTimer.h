#pragma once
#include <functional>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>

class TaskTimer
{
public:
    explicit TaskTimer();
    ~TaskTimer();

public:
    void Start(int interval, std::function<void()> task);
    void StartOnce(int interval, std::function<void()> task);
    void Stop();
    bool isRunning();

private:
    std::atomic<bool> _expired;
    std::atomic<bool> _try_to_expire;
    std::mutex _mutex;
    std::condition_variable _expired_cond;
};

