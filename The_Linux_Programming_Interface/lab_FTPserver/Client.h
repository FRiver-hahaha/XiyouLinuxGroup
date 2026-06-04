#ifndef CLIENT_H
#define CLIENT_H

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>

using std::string; 
using std::cout;    using std::cin;

constexpr int SIZE_BUFFER = 4096;
constexpr int FTP_CONTROL_PORT = 2100; 

class Client {
    int clientSocket;                 // 控制连接套接字
    int serverPort;                   // 服务器端口
    string serverAddress;             // 服务器地址
    int dataSocket;                   // 数据连接套接字
    int pasvPort;                     // 被动模式端口
    string pasvHost;                  // 被动模式主机

public:
    Client(const string& address = "127.0.0.1", int port = FTP_CONTROL_PORT)
     : clientSocket(-1), serverPort(port), serverAddress(address),
       dataSocket(-1), pasvPort(-1) {}

    bool connectServer();             // 连接到服务器
    void receiveResponse();           // 接收响应
    string sendCommand(const string& command);  // 发送命令
    void microShell();                // 交互模式
    
    bool connectFtpServer();          // 连接 FTP 服务器
    string sendFtpCommand(const string& command);  // 发送 FTP 命令
    bool pasvMode();                  // 进入被动模式
    string listFiles();               // 列出文件
    bool downloadFile(const string& filename);     // 下载文件
    bool uploadFile(const string& filename);       // 上传文件
    void closeDataConnection();       // 关闭数据连接
    void disconnect();                // 断开连接

    ~Client() {
        disconnect();
    }
};

#endif