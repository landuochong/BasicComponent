#ifndef TIMER_H
#define TIMER_H

#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>

class Timer
{
public:
    class Listener
    {
    public:
        virtual ~Listener() = default;

    public:
        virtual void OnTimer(Timer* timer) = 0;
    };

public:
    explicit Timer(Listener* listener);
    Timer& operator=(const Timer&) = delete;
    Timer(const Timer&)            = delete;
    ~Timer();

public:
    void Start(int timeout_ms, int repeat = 0);
    void Stop();
    int GetTimeout() const
    {
        return this->timeout;
    }
    int GetRepeat() const
    {
        return this->repeat;
    }

    bool IsActive() const
    {
        return !_expired;
    }

private:
    // Passed by argument.
    Listener* listener{ nullptr };

    // Others.
    std::atomic<bool> _expired;
    std::atomic<bool> _try_to_expire;
    std::mutex _mutex;
    std::condition_variable _expired_cond;

    int timeout{ 0u };
    int repeat{ 0u };
};

#endif // TIMER_H
