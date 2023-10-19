#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include "thread/thread_types.h"
namespace basic_comm{
    class ThreadPool {
    public:
        ThreadPool(std::string name,size_t);
        ~ThreadPool();

        /**
         * two method to call class method:
         * bind： .enqueue(std::bind(&Dog::sayHello, &dog));
         * mem_fn： .enqueue(std::mem_fn(&Dog::sayHello), this)
         */
        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;

		template<class F, class... Args>
		auto enqueue_first(F&& f, Args&&... args)
			->std::future<typename std::result_of<F(Args...)>::type>;

        size_t getSize(){
            return workers.size();
        }

        void stopPool() {
            stop = true;
            condition.notify_one();
        }

        bool hasStop() {
            return stop;
        }

        void clearTask(){
            if(stop){
                return;
            }
            {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                tasks.clear();
            }
        }

        size_t getTaskSize(){
            return tasks.size();
        }
    private:
        // need to keep track of threads so we can join them
        std::vector< std::thread > workers;
        size_t threadNum;
        // the task queue
        std::deque< std::function<void()> > tasks;

        // synchronization
        std::mutex queue_mutex;
        std::condition_variable condition;
        std::string name;

        volatile bool stop;
    };

    // the constructor just launches some amount of workers
    inline ThreadPool::ThreadPool(std::string name, size_t threads)
        :   stop(false)
    {
        this->name = name;
        threadNum = threads;

        //std::cout << "CSTThreadPool name="<<name<< std::endl;
        for(size_t i = 0;i<threads;++i){
            workers.emplace_back(
                        [name, this]
            {
                if (threadNum == 1) {
                    SetCurrentThreadName(name.c_str());
                }
                while(true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                                             [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop || this->tasks.empty()){
                            return;
                        }
                        task = std::move(this->tasks.front());
                        this->tasks.pop_front();
                    }
                    //try{
                        //std::cout << "start name=" << name << "tid=" << std::this_thread::get_id() << std::endl << std::flush;
                        task();
                        if(this->stop){
                            return;
                        }
                        //std::cout << "end tid=" << std::this_thread::get_id() << std::endl << std::flush;
                    //}catch(std::exception e){
                    //   std::cout << e.what() << std::flush;
                    //}
                }
            }
            );
        }
    }


    // add new work item to the pool
    template<class F, class... Args>
    auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared< std::packaged_task<return_type()> >(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                    );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            // don't allow enqueueing after stopping the pool
            if(stop){
                return std::async([=]
                         {
                           return;
                         });
            }
            tasks.emplace_back([task](){ (*task)(); });
        }
        condition.notify_one();
        return res;
    }

	template<class F, class... Args>
    auto ThreadPool::enqueue_first(F&& f, Args&&... args)
		-> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			// don't allow enqueueing after stopping the pool
			if (stop) {
				return std::async([=]
					{
						return;
					});
			}
			tasks.emplace_front([task]() { (*task)(); });
		}
		condition.notify_one();
		return res;
	}

    // the destructor joins all threads
    inline ThreadPool::~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for(std::thread &worker: workers)
            worker.join();
    }
}
#endif
