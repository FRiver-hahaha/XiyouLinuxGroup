// 2026.4.13 完成了基本的类内声明，实现了日志处理函数，支持可回调的任务提交函数

// 明天完成worker和submit函数

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

/*
    日志处理函数:
    接受字符串参数，获取当前时间，打印日志。
*/

inline void log(const string& msg) {
    static std::mutex logMtx;
    std::lock_guard<std::mutex> lg(logMtx);

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    cout << "[log " << std::put_time(std::localtime(&time), "%H:%M:%S")
         << "]" << msg << endl;
}

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

    template<class F, class... Args, class callBack>
    auto submit(F&& f, Args&&... args, callBack&& cb)
                -> std::future<typename std::result_of<F(Args...)>::type>;
      
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

/*
    线程池构造函数:
    将创建的线程加入到vector里
*/

ThreadPool::ThreadPool(size_t TNum) {
    for(size_t i = 0; i < TNum; ++i) {
        workers.emplace_back(&ThreadPool::worker, this);
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
}