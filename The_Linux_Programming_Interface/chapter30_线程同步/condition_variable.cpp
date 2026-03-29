#include <iostream>
#include <thread>
#include <condition_variable>
using namespace std;

mutex mtx;
condition_variable cv;
bool ready = false;

void consumer() {
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [] { return ready; });

    cout << "消费开始" << endl;
}

void producer() {
    lock_guard<mutex> lock(mtx);

    ready = true;

    cv.notify_one();
}

int main() {
    thread t1(consumer);
    thread t2(producer);

    t1.join();
    t2.join();

    return 0;
}

/*
线程的创建是线性的，线程的执行是同步的
*/