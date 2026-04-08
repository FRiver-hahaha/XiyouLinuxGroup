#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>

class ThreadPool {
public:
    // 构造函数：创建 threadNum 个工作线程
    ThreadPool(size_t threadNum);

    // 析构函数：保证所有任务执行完毕再退出
    ~ThreadPool();

    // 提交任务到线程池
    // task: 要执行的任务
    // callback: 任务完成后的回调（可选）
    void submit(std::function<void()> task, std::function<void()> callback = nullptr);

private:
    // 工作线程执行的函数
    void worker();

private:
    std::vector<std::thread> workers;       // 工作线程
    std::queue<std::function<void()>> tasks; // 任务队列

    std::mutex mtx;                   // 保护队列
    std::condition_variable cv;       // 线程等待/唤醒
    bool stop = false;                // 线程池关闭标志

    // 任务 + 回调 打包
    struct TaskWithCallback {
        std::function<void()> task;
        std::function<void()> callback;
    };
    std::queue<TaskWithCallback> taskQueue; // 带回调的任务队列
};

// ==================== 实现 ====================

inline ThreadPool::ThreadPool(size_t threadNum) {
    for (size_t i = 0; i < threadNum; ++i) {
        workers.emplace_back(&ThreadPool::worker, this);
    }
}

inline ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all(); // 唤醒所有线程

    for (auto& t : workers) {
        if (t.joinable())
            t.join();
    }
}

// 提交任务（带可选回调）
inline void ThreadPool::submit(std::function<void()> task, std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(mtx);
    taskQueue.push({std::move(task), std::move(callback)});
    cv.notify_one(); // 唤醒一个线程
}

// 工作线程主循环
inline void ThreadPool::worker() {
    while (true) {
        TaskWithCallback twc;

        {
            std::unique_lock<std::mutex> lock(mtx);

            // 等待：有任务 或 线程池关闭
            cv.wait(lock,  {
                return stop || !taskQueue.empty();
            });

            // 关闭且无任务，退出
            if (stop && taskQueue.empty())
                return;

            // 取任务
            twc = std::move(taskQueue.front());
            taskQueue.pop();
        }

        // 执行任务
        if (twc.task)
            twc.task();

        // 执行回调
        if (twc.callback)
            twc.callback();
    }
}