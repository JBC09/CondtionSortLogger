//
// Created by chanbin on 25. 7. 28.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <functional>
#include <atomic>
#include <future>
#include <type_traits>
#include <vector>

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
public:
    std::atomic<bool> stop;

    explicit ThreadPool(std::size_t threadCount = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result_t<F,Args...>> {

        using return_type = std::invoke_result_t<F,Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            [f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
                return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
            }
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            if(stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([task]() { (*task)(); });
        }

        condition.notify_one();
        return res;
    }


};





#endif //THREADPOOL_H
