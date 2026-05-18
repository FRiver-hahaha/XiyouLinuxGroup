#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <atomic>
#include <iostream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <functional>
#include <iomanip>
#include <vector>   
#include "Store.h"
#include "CommandHandler.h"
#include "ThreadPool.h"

using std::cout;    using std::endl;    using std::cin;
using std::string;  using hash_map = std::unordered_map<string, string>;
using std::mutex;   using std::unique_ptr;

constexpr int SIZE_BUFFER = 1024;

class Server {

    int serverSocket;
    int port;
    std::vector<int> clientSockets;
    std::atomic<bool> running{true};
    unique_ptr<ThreadPool> threadPool;
    mutex clientMutex;
    
    Store store;

    CommandHandler cmdHandle{store};
    
    std::thread ServerThread;

    std::function<void(const string&)> logCallback;      // 日志回调
    std::function<void(int)> clientCountCallback;        // 客户端数量变化回调
    std::function<void(const string&, const string&)> commandCallback; // 命令执行回调

public:
    Server(int port = 8888);
    
    /* 传输层封装实现 */ 
    bool start();// 用来监听客户端
    void run();// 用来建立和客户端的链接
    void stop();// 用来关闭连接
    void talkWithClient(const int clientSock);// 与客户端进行i/o
    void removeClient(const int clientSocket);// 把客户端移除
    std::vector<string> getLocalIps();
    string parseCommand(const string& command);

    /* 应用层封装实现 */ 
    void printServerInfo();
    
    // Qt 控制接口
    void setLogCallback(std::function<void(const string&)> callback) { logCallback = callback; };
    void setClientCountCallback(std::function<void(int)> callback) { clientCountCallback = callback; };
    void setCommandCallback(std::function<void(const string&, const string&)> callback) { commandCallback = callback; };
    
    // 获取私有变量的调用
    int getClientCount();
    bool isRunning() const {return running; };
    int getPort() const {return port; };

    ~Server() {
        stop();
    }
};

#endif
