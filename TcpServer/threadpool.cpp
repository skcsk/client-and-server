#include "threadpool.h"

ThreadPool::ThreadPool():stop(false)
{
    for (int i = 0; i < THREAD_SIZE; ++i) {

        threads.emplace_back([this](){
            while(1){
                std::unique_lock<std::mutex>lock(mtx);

                cv.wait(lock,[=](){
                    return stop||!tasks.empty();
                });
                if(stop&&tasks.empty()){
                    return;
                }
                auto task = move(tasks.front());
                tasks.pop();
                lock.unlock();
                task();
            }
        });
    }

}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex>lock(mtx);
        stop = true;

    }
    cv.notify_all();
    for(auto &t:threads){
        t.join();
    }

}

ThreadPool &ThreadPool::getInstance()
{
    static ThreadPool instance;
    return instance;
}



