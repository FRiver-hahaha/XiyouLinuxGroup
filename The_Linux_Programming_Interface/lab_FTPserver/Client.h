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
#include <vector>
#include <tuple>

using std::string; 
using std::cout;    using std::cin;

constexpr int SIZE_BUFFER = 4096;
constexpr int FTP_CONTROL_PORT = 2100; 

struct PendingFile {
    string filename;
    string hash;
    off_t uploadedBytes;
};

class Client {
    int clientSocket;                 // 控制连接套接字
    int serverPort;                   // 服务器端口
    string serverAddress;             // 服务器地址
    int dataSocket;                   // 数据连接套接字
    int pasvPort;                     // 被动模式端口
    string pasvHost;                  // 被动模式主机
    string currentDir;                // 当前工作目录

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
    string sendCommandWithMultiResponse(const string& command);  // 【新增】发送命令并接收多行响应
    string getHiddenInput(const string& prompt);  // 【新增】隐藏密码输入
    bool pasvMode();                  // 进入被动模式
    string listFiles();               // 列出文件
    bool downloadFile(const string& filename);     // 下载文件
    bool uploadFile(const string& filename, off_t offset);  // 【新增】上传文件（支持偏移量）
    bool uploadFileWithLocalPath(const string& localPath, const string& remoteFilename, off_t offset);  // 【修复】使用本地路径上传
    void closeDataConnection();       // 关闭数据连接
    void disconnect();                // 断开连接
    
    void fetchPendingFiles();         // 获取未完成文件列表
    PendingFile* findPendingFile(const string& filename);  // 查找未完成文件
    void showMainMenu();              // 显示主菜单
    void updateCurrentDir();          // 【新增】更新当前目录
    string getCurrentDir() const { return currentDir; }  // 获取当前目录
    bool login();                     // 用户登录
    void tuiUploadFile();             // TUI 上传文件（带续传选择）
    void tuiMainLoop();               // TUI 主循环

    ~Client() {
        disconnect();
    }
};

#endif
