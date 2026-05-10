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
#include <memory>
#include <string>

using std::string, std::cout, std::endl;

inline void log(const string& msg) {
    static std::mutex logMtx;
    std::lock_guard<std::mutex> lg(logMtx);
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::cout << std::put_time(std::localtime(&now_time_t), "%H:%M:%S")
              << "." << std::setfill('0') << std::setw(3) << ms.count()
              << " " << msg << std::endl;
}

class ThreadPool {
public:
    ThreadPool(size_t TNum) : stop(false) {
        log("(init) Thread pool created with " + std::to_string(TNum) + " threads");
        for(size_t i = 0; i < TNum; ++i) {
            workers.emplace_back(&ThreadPool::worker, this);
        }
    }
    
    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lg(mtx);
            stop = true;
        }
        cv.notify_all();
        for(auto& t : workers) {
            if(t.joinable()) t.join();
        }
        log("(finish) Thread pool destroyed");
    }

    template<class F, class CB>
    auto submit(F&& f, CB&& cb) -> std::future<decltype(f())> {
        using returnType = decltype(f());
        auto job = std::make_shared<std::packaged_task<returnType()>>(std::forward<F>(f));
        std::future<returnType> res = job->get_future();
        {
            std::lock_guard<std::mutex> lg(mtx);
            jobs.emplace([job, cb]() {
                (*job)();
                cb();
            });
        }
        cv.notify_one();
        return res;
    }
    
    void waitAll() {
        std::unique_lock<std::mutex> ul(mtx);
        cv.wait(ul, [this]() { return jobs.empty(); });
    }

private:
    void worker() {
        log("(worker) Thread started");
        while(true) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> ul(mtx);
                cv.wait(ul, [this]() {return stop || !jobs.empty(); });
                if(stop && jobs.empty()) {
                    log("(worker) Thread stopped");
                    return;
                }
                job = std::move(jobs.front());
                jobs.pop();
            }
            job();
        }
    }
    
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobs;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop;
};

#endif