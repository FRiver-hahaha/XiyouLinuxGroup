#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <vector>
#include <condition_variable>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <chrono>
#include <future>
#include <iomanip>

using std::string, std::cout, std::endl;


class ThreadPool {

/*
    公共接口:构造函数和析构函数，
    自动初始化线程池，
    并且在工作完成之后结束线程池。
    定义任务提交函数。
    使用模板，接受任意可调用对象，任意对象的参数，回调函数。
    使用auto处理不确定的返回值，
    异步执行任务，自动执行回调函数，
    最后返回future对象，用来同步等待任务结果。
*/

public:
    ThreadPool(size_t TNum);
    ~ThreadPool();

    template<class F, class CB>
    auto submit(F&& f, CB&& cb) -> std::future<decltype(f())>;
      
/*
    隐藏接口:
    定义工作线程以及存放线程的数组，任务队列
    锁，条件变量，控制线程之间调度的条件
*/

private:
    void worker();// 工作线程
    std::vector<std::thread> workers;// 线程
    std::queue<std::function<void()>> jobs;// 任务队列

    std::mutex mtx;// 锁
    std::condition_variable cv;// 条件变量
    bool stop = false;// 控制条件
};

inline void log(const string& msg) {
    static std::mutex logMtx;
    std::lock_guard<std::mutex> lg(logMtx);

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::cout << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] " << msg << std::endl;
}

inline void log(const string& msg, int x) {
    static std::mutex logMtx;
    std::lock_guard<std::mutex> lg(logMtx);

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::cout << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] " << msg << " " << x << std::endl;
}

/*
    线程池任务提交函数:
    模板:声明，接受任意任务，参数，回调
    返回值:future异步处理任务结果，最后执行回调
    returnType:任务自动获取到对应任务的返回值，用于回调
    job:任务包装，加入到队列中
    上锁访问队列:加入
*/

template<class F, class CB>
auto ThreadPool::submit(F&& f, CB&& cb) -> std::future<decltype(f())> {
    using returnType = decltype(f());// 获取返回值类型

    auto job = std::make_shared<std::packaged_task<returnType()>> (
        std::forward<F>(f)
    );// 任务包装器:在让多线程共享的前提下，将任务完美包装到job里
    std::future<returnType> res = job->get_future();// 获取结果

    {// 上锁访问队列
        std::lock_guard<std::mutex> lg(mtx);
        jobs.emplace([job, cb]() {
            (*job)();
            cb();
        });
    }
    cv.notify_one();// 唤醒线程，准备工作
    log("(submit)任务已加入队列");
    return res;
}

#endif