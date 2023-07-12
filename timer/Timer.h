#pragma once
#include <functional>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>

class Timer
{
public:
    explicit Timer();
    ~Timer();

public:
    void start(int interval, std::function<void()> task);
    void startOnce(int interval, std::function<void()> task);
    void stop();
    bool isRunning();

private:
    std::atomic<bool> _expired;
    std::atomic<bool> _try_to_expire;
    std::mutex _mutex;
    std::condition_variable _expired_cond;
};

