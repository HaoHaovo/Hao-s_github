#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <assert.h>

class ThreadPool
{
public:
    ThreadPool() = default;
    ThreadPool(ThreadPool &&) = default;
    explicit ThreadPool(int threadcount = 8) : pool_(std::make_shared<Pool>())
    {
        assert(threadcount > 0);
        for (int i = 0; i < threadcount; ++i)
        {
            std::thread([this]()
                        { std::unique_lock<std::mutex> locker(pool_->mtx_); 
                        while(true){
                            if(!pool_->tasks.empty()){
                                auto task = std::move(pool_->tasks.front());
                                pool_->tasks.pop();
                                locker.unlock();
                                task();
                                locker.lock();
                            }else if(pool_->isClosed){
                                break;
                            }else {
                                pool_->cond_.wait(locker);
                            }
                        } })
                .detach();
        }
    }
    ~ThreadPool()
    {
        if (pool_)
        {
            std::unique_lock<std::mutex> locker(pool_->mtx_);
            pool_->isClosed = true;
        }
        pool_->cond_.notify_all();
    }
    template <typename T>
    void AddTask(T &&task)
    {
        std::unique_lock<std::mutex> locker(pool_->mtx_);
        pool_->tasks.emplace(std::forward<T>(task));
        pool_->cond_.notify_one();
    }

private:
    struct Pool
    {
        std::mutex mtx_;
        std::condition_variable cond_;
        bool isClosed;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_;
};

#endif