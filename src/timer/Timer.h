#pragma once
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <qDebug>

#include<ctime>
#include <condition_variable>

class Timer
{
private:
	std::atomic<bool> expired_;
	std::atomic<bool> try_to_expire_;
	std::mutex mtx;
	std::condition_variable cv;
	
	int interval_= 0;
	bool hasStarted = false;	

public:
	Timer() : 
	expired_(true),
	try_to_expire_(false),
	interval_(0),
	hasStarted(false)
	{
		
	}

	Timer(const Timer& timer)
	{
		expired_ = timer.expired_.load();
		try_to_expire_ = timer.try_to_expire_.load();
	}

	~Timer()
	{
		stop();
	}

	void start(int interval, std::function<void()> &task)
	{
		if (!expired_)
			return;

		hasStarted = true;
		expired_ = false;
		interval_ = interval;
		std::thread([this, interval, task]() {
			clock_t startTime = clock();
			while (!try_to_expire_)
			{
				double time = clock() - startTime;
				if ( time < interval_ )
				{
					//every 20ms impl once
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
					continue;
				}
				else
				{
					if (!try_to_expire_)
					{
						qDebug() << " timer task_ exec++++++++++++++++++++++++++++++++++++++++++++++++++\n";
						task();
						startTime = clock();
					}
				}
			}
			std::lock_guard<std::mutex> lock(mtx);
			expired_ = true;
			cv.notify_one();
		}).detach();
	}

	void stop()
	{
		if (expired_)
			return;

		if (try_to_expire_)
			return;

		try_to_expire_ = true;
		
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this] {return expired_ == true; });

		if (expired_)
			try_to_expire_ = false;
		task_ = nullptr;
		hasStarted = false;
	}

	bool HasStart()
	{
		return hasStarted;
	}
};

