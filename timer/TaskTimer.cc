#include "TaskTimer.h"
#include <thread>

TaskTimer::TaskTimer()
    : _expired(true)
    , _try_to_expire(false)
{
}

TaskTimer::~TaskTimer()
{
    Stop();
}

void TaskTimer::Start(int interval_ms, std::function<void()> task)
{
    // is started, do not start again
    if (_expired == false)
        return;

    // start async timer, launch thread and wait in that thread
    _expired = false;
    _try_to_expire = false;
    std::thread([this, interval_ms, task]() {
        int count = 0;
        while (!_try_to_expire)
        {
            // sleep every interval and do the task again and again until times up
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if(_try_to_expire){
                break;
            }
            ++count;
            if (count >= interval_ms / 100)
            {
                task();
                count = 0;
            }
        }

        {
            // timer be stopped, update the condition variable expired and wake main thread
            std::lock_guard<std::mutex> locker(_mutex);
            _expired = true;
            _expired_cond.notify_one();
        }
    }).detach();

}

void TaskTimer::StartOnce(int interval_ms, std::function<void()> task)
{
    // is started, do not start again
    if (_expired == false)
        return;

    // start async timer, launch thread and wait in that thread
    _expired = false;
    _try_to_expire = false;
    std::thread([this, interval_ms, task]() {
        int count = 0;
        while (!_try_to_expire)
        {
            // sleep every interval and do the task again and again until times up
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if(_try_to_expire){
                break;
            }
            ++count;
            if (count >= interval_ms / 100)
            {
                task();
                break;
            }
        }

        {
            // timer be stopped, update the condition variable expired and wake main thread
            std::lock_guard<std::mutex> locker(_mutex);
            _expired = true;
            _expired_cond.notify_one();
        }
    }).detach();
}

void TaskTimer::Stop()
{
    // do not stop again
    if (_expired)
        return;

    if (_try_to_expire)
        return;

    // wait until timer 
    _try_to_expire = true; // change this bool value to make timer while loop stop
    {
        std::unique_lock<std::mutex> locker(_mutex);
        _expired_cond.wait(locker, [this] {return _expired == true; });

        // reset the timer
        if (_expired == true)
            _try_to_expire = false;
    }

}

bool TaskTimer::isRunning()
{
    return _expired == false;;
}
