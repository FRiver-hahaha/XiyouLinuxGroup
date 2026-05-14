#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t TNum) {
    log("(init)初始化线程池,使用 " + std::to_string(TNum) + " 个线程");
    for(size_t i = 0; i < TNum; ++i) {
        workers.emplace_back(&ThreadPool::worker, this);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
}

/*
    线程池析构函数:
    将所有线程进行回收
*/

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lg(mtx);// 上锁，通知所有线程即将关闭线程池
        stop = true;
    }
    cv.notify_all();// 唤醒所有线程

    for(auto& t : workers) {
        if(t.joinable()) t.join();
    }
    log("(finish)线程池已关闭");
}

/*
    线程池工作函数:
    while循环:等待任务，期间处于休眠状态，除非线程池关闭
    锁:访问队列时加锁，执行任务时解锁
    条件变量:没有任务时休眠，有任务时加锁
    任务:如果在任务队列中检测到有任务，被唤醒之后，从队列中取出，执行任务
*/

void ThreadPool::worker() {
    log("(worker)工作线程");
    while(true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> ul(mtx);
            cv.wait(ul, [this]() {return stop || !jobs.empty(); });

            if(stop && jobs.empty()) {
                log("(worker)工作线程已销毁");
                return;
            }
            job = std::move(jobs.front());
            jobs.pop();
        }
        job();
    }
}
