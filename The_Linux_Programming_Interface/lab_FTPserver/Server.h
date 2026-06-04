#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h> 
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
#include <fstream> 
#include <dirent.h>
#include <sys/stat.h> 
#include <cmath>
#include "Store.h"
#include "CommandHandler.h"
#include "ThreadPool.h"

using std::cout;    using std::endl;    using std::cin;
using std::string;  using hash_map = std::unordered_map<string, string>;
using std::mutex;   using std::unique_ptr;

constexpr int SIZE_BUFFER = 4096;       // 缓冲区
constexpr int FTP_CONTROL_PORT = 2100;  // 控制端口
constexpr int FTP_DATA_PORT = 2000;     // 主动模式数据端口
constexpr int MAX_EPOLL_EVENTS = 10;    // epoll 最大事件数
constexpr int PASV_PORT_BASE = 5000;    // 被动模式端口起始值

struct FtpSession {
    int controlSock = -1;           // 控制连接套接字
    int dataSock = -1;              // 数据连接套接字
    int pasvListenSock = -1;        // PASV 监听套接字（在 PASV 命令时创建）
    int pasvPort = -1;              // 被动模式端口
    string clientIp = "";           // 客户端 IP
    string workingDir = "/";        // 当前工作目录
    bool pasvMode = false;          // 是否被动模式
};

class Server {
    int controlServerSocket;                    // 控制连接监听套接字
    int dataServerSocket;                       // 数据连接监听套接字（主动模式）
    int epollFd;                                // epoll 
    int controlPort;                            // 控制端口
    int dataPort;                               // 数据端口
    std::atomic<bool> running{true};            // 运行状态
    unique_ptr<ThreadPool> threadPool;          // 线程池
    mutex sessionMutex;                         // 会话锁
    mutex dataPortMutex;                        // 数据端口分配锁
    std::unordered_map<int, FtpSession*> sessions;  // 套接字映射
    std::string serverIp;                           // 服务器 IP
    int nextPasvPort;                               // 下一个被动端口

public:
    Server(int controlPort = FTP_CONTROL_PORT, int dataPort = FTP_DATA_PORT);
    
    /* 传输层封装实现 */ 
    bool start();         // 用来监听客户端
    void run();           // 用来建立和客户端的链接
    void stop();          // 用来关闭连接

    std::vector<string> getLocalIps();  // 获取本地 IP
    
    void handleClientData(int clientSock);                         // 处理客户端数据
    void handleFtpCommand(int clientSock, const string& command);  // 处理 FTP 命令
    void handlePasvCommand(int clientSock);                        // 处理 PASV 被动模式命令
    void handleListCommand(int clientSock);                        // 处理 LIST 列目录命令
    void handleRetrCommand(int clientSock, const string& filename); // 处理 RETR 下载命令
    void handleStorCommand(int clientSock, const string& filename); // 处理 STOR 上传命令
    void handleCwdCommand(int clientSock, const string& path);     // 处理 CWD 切换目录命令
    void handleCdupCommand(int clientSock);                        // 处理 CDUP 返回上级命令
    void handleShutdownCommand(int clientSock);                    // 处理 SHUTDOWN 关闭服务器命令

    void sendDataToClient(int dataSock, const string& data);       // 发送数据到客户端
    string receiveDataFromClient(int dataSock, size_t maxBytes);   // 从客户端接收数据
    string listDirectory(const string& path);                      // 列出目录内容
    bool saveFile(const string& path, const string& content);      // 保存文件
    string readFile(const string& path);                           // 读取文件
    void removeSession(int clientSock);                            // 移除会话
    string getServerIp();                                          // 获取服务器 IP
    string getWorkingDir(int clientSock);                          // 获取工作目录
    int allocatePasvPort();                                        // 分配被动模式端口
    void releasePasvPort(int port);                                // 释放被动模式端口

    /* 应用层封装实现 */ 
    void printServerInfo();  // 打印服务器信息
    
    // 获取私有变量的调用
    bool isRunning() const {return running;};
    int getControlPort() const {return controlPort;};  // 获取控制端口
    int getSessionCount();                             // 获取会话数

    ~Server() {
        stop();
    }
};

#endif