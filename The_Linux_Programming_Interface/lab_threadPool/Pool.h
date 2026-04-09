#include <iostream>
#include <vector>
#include <condition_variable>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>

class ThreadPool {
public:
    ThreadPool(size_t TNum);
    ~ThreadPool();

    void submit(std::function<void()> task, 
    std::function<void()> callback = nullptr);
                
private:
    void worker();

private:

}