#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <future>
#include <chrono>
#include <iomanip>

// 日志打印工具（线程安全）
inline void log(const std::string& msg) {
    static std::mutex log_mtx;
    std::lock_guard<std::mutex> lock(log_mtx);

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::cout << "[日志 " << std::put_time(std::localtime(&time), "%H:%M:%S") << "] "
              << msg << std::endl;
}

class ThreadPool {
public:
    ThreadPool(size_t threadNum);
    ~ThreadPool();

    // 提交任务：支持返回值 + 明确回调 + 日志
    template<class F, class... Args, class Callback>
    auto submitWithCallback(F&& f, Args&&... args, Callback&& callback)
        -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    void worker();

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;
};

// 构造函数
inline ThreadPool::ThreadPool(size_t threadNum) {
    log("线程池初始化，线程数：" + std::to_string(threadNum));
    for (size_t i = 0; i < threadNum; ++i) {
        workers.emplace_back(&ThreadPool::worker, this);
    }
}

// 析构函数
inline ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for (auto& t : workers) {
        if (t.joinable()) t.join();
    }
    log("线程池已安全关闭");
}

// 工作线程
inline void ThreadPool::worker() {
    log("工作线程启动");
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]() { return stop || !tasks.empty(); });

            if (stop && tasks.empty()) {
                log("工作线程退出");
                return;
            }
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

// 提交任务（高级版：返回值 + 回调 + 日志）
template<class F, class... Args, class Callback>
auto ThreadPool::submitWithCallback(F&& f, Args&&... args, Callback&& callback)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<return_type> res = task->get_future();

    {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.emplace([task, callback]() {
            (*task)();
            callback();
            log("任务执行完成并触发回调");
        });
    }
    cv.notify_one();
    log("新任务已加入队列");
    return res;
}