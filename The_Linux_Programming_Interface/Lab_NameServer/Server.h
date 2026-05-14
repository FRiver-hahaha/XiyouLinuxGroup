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
    std::vector<int> clientSockets;
    int port;
    std::atomic<bool> running{true};
    Store store;
    CommandHandler cmdHandle{store};

    unique_ptr<ThreadPool> threadPool;
    mutex clientMutex;
    

    std::thread ServerThread;

public:
    Server(int port = 8888);

    bool start();// 用来监听客户端
    void run();// 用来建立和客户端的链接
    void stop();// 用来关闭连接
    void talkWithClient(const int clientSock);// 与客户端进行i/o
    void removeClient(const int clientSocket);// 把客户端移除

    void console();
    void printClientList();
    void printServerInfo();
    string parseCommand(const string& command);
    std::vector<string> getLocalIps();

    ~Server() {
        stop();
    }
};

#endif
