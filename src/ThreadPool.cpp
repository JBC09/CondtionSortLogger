//
// Created by chanbin on 25. 7. 28.
//

#include "../include/ThreadPool.h"

ThreadPool::ThreadPool(std::size_t threadCount) : stop(false)
{
    for (int i = 0; i < threadCount; ++i)
    {
        workers.emplace_back([this] {
            while (true)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this]
                    {
                        return this->stop || !this->tasks.empty();
                    });

                    if (this->stop && this->tasks.empty())
                    {
                        return;
                    }

                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        });
    }
};


ThreadPool::~ThreadPool()
{
    stop = true;

    condition.notify_one();

    for (std::thread& worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}


