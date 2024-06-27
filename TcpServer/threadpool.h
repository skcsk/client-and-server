#ifndef THREADPOOL_H
#define THREADPOOL_H
#include<thread>
#include<mutex>
#include<condition_variable>
#include<chrono>
#include<vector>
#include<queue>
#include<functional>
#include<utility>
#define THREAD_SIZE 4
class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();
    static ThreadPool &getInstance();

    template<class F,class ...Agrs>
    void addTask(F&&f,Agrs ...agrs)
    {
        auto stak = std::bind(std::forward<F>(f), std::forward<Agrs>(agrs)...);
        {
            std::unique_lock<std::mutex>lock(mtx);
            tasks.emplace(stak);
        }
        cv.notify_one();
        return;
    }


private:
    std::mutex mtx;
    std::condition_variable cv;

    bool stop;
    //任务队列
    std::queue<std::function<void()>> tasks;
    //thread队列
    std::vector<std::thread> threads;
};

#endif // THREADPOOL_H
