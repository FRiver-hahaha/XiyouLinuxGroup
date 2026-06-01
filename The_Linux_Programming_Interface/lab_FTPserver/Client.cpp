#include "Client.h"
#include <sstream>
#include <termios.h>
#include <iomanip>

static std::vector<PendingFile> g_pendingFiles;

// ANSI 颜色代码
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"


bool Client::connectFtpServer() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::runtime_error("无法创建套接字");
        return false;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);  // 控制端口 2100

    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        std::runtime_error("无法获取 IP 地址");
        close(clientSocket);
        return false;
    }

    if (::connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::runtime_error("无法连接到服务器");
        close(clientSocket);
        return false;
    }

    cout << GREEN << "✓ 已连接到 FTP 服务器 " << serverAddress << ":" << serverPort << RESET << '\n';

    // 接收欢迎消息
    receiveResponse();
    
    return true;
}

void Client::receiveResponse() {
    char buffer[SIZE_BUFFER];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    if(bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        cout << CYAN << buffer << RESET;
    }
}

string Client::sendFtpCommand(const string& command) {
    if (clientSocket == -1) {
        return "无法连接到服务器";
    }

    string cmd = command + "\r\n";
    send(clientSocket, cmd.c_str(), cmd.length(), 0);

    char buffer[SIZE_BUFFER];
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        string response(buffer);
        // 移除末尾的\r\n
        while(!response.empty() && (response.back() == '\r' || response.back() == '\n')) {
            response.pop_back();
        }
        return response;
    }
    
    return "链接已被服务器关闭";
}

// 【新增】发送命令并接收多行响应
string Client::sendCommandWithMultiResponse(const string& command) {
    string resp = sendFtpCommand(command);
    string fullResponse = resp + "\n";
    
    // 继续读取直到 220 END 或 226 等结束标志
    while(resp.find("220 END") == string::npos && 
          resp.find("226") == string::npos &&
          resp.find("213") == string::npos) {
        char buffer[SIZE_BUFFER];
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if(bytes_received <= 0) break;
        
        buffer[bytes_received] = '\0';
        resp = buffer;
        fullResponse += resp + "\n";
    }
    
    return fullResponse;
}

// 【新增】隐藏密码输入
string Client::getHiddenInput(const string& prompt) {
    cout << prompt;
    string input;
    
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    getline(cin, input);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    cout << '\n';
    
    return input;
}

// 【新增】获取未完成文件列表
void Client::fetchPendingFiles() {
    g_pendingFiles.clear();
    string resp = sendFtpCommand("RESUME");
    
    // 【修复】添加响应检查，防止空响应或未认证导致崩溃
    if(resp.empty()) {
        return;
    }
    
    // 【修改】检查响应是否包含 220，并处理单行响应情况（如"220 没有未完成的文件"）
    if(resp.find("220") == string::npos) {
        // 如果是 530 未认证，忽略
        if(resp.find("530") == string::npos) {
            cout << YELLOW << "⚠ RESUME 命令响应异常：" << resp << RESET << '\n';
        }
        return;
    }
    
    // 【修改】如果响应已经是完整单行（包含 END 或不包含续传文件标记），直接返回
    if(resp.find("220 END") != string::npos || resp.find("没有未完成的文件") != string::npos) {
        return;
    }
    
    // 解析多行响应
    while(true) {
        char buffer[SIZE_BUFFER];
        memset(buffer, 0, sizeof(buffer));
        int received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if(received <= 0) break;
        
        buffer[received] = '\0';
        string line(buffer);
        
        if(line.find("220 END") != string::npos) break;
        
        // 解析：1. filename (已上传：xxx 字节，MD5:xxx...)
        size_t dotPos = line.find(". ");
        size_t parenPos = line.find(" (已上传：");
        
        if(dotPos != string::npos && parenPos != string::npos) {
            PendingFile pf;
            pf.filename = line.substr(dotPos + 2, parenPos - dotPos - 2);
            
            size_t bytesPos = line.find("已上传：") + 4;
            size_t bytesEnd = line.find(" 字节", bytesPos);
            if(bytesPos != string::npos && bytesEnd != string::npos) {
                pf.uploadedBytes = stoll(line.substr(bytesPos, bytesEnd - bytesPos));
            } else {
                pf.uploadedBytes = 0;
            }
            
            size_t hashPos = line.find("MD5:");
            if(hashPos != string::npos) {
                size_t hashEnd = line.find("...", hashPos);
                if(hashEnd != string::npos) {
                    pf.hash = line.substr(hashPos + 4, hashEnd - hashPos - 4);
                }
            }
            
            g_pendingFiles.push_back(pf);
        }
    }
}

// 【新增】查找是否有未完成的同名文件
PendingFile* Client::findPendingFile(const string& filename) {
    for(auto& pf : g_pendingFiles) {
        if(pf.filename == filename) {
            return &pf;
        }
    }
    return nullptr;
}

bool Client::pasvMode() {
    string response = sendFtpCommand("PASV");
    
    //227 entering passive mode (h1,h2,h3,h4,p1,p2)
    size_t start = response.find('(');
    size_t end = response.find(')');
    
    if(start == string::npos || end == string::npos) {
        cout << "PASV 响应格式错误\n";
        return false;
    }
    
    string params = response.substr(start + 1, end - start - 1);
    
    // h1,h2,h3,h4,p1,p2
    int h1, h2, h3, h4, p1, p2;
    sscanf(params.c_str(), "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2);
    
    //port = p1*256 + p2
    pasvPort = p1 * 256 + p2;
    pasvHost = std::to_string(h1) + "." + std::to_string(h2) + "." + 
               std::to_string(h3) + "." + std::to_string(h4);
    
    cout << "被动模式：" << pasvHost << ":" << pasvPort << '\n';
    return true;
}

string Client::listFiles() {
    // 先进入被动模式，获取数据端口
    if(!pasvMode()) {
        return "无法进入被动模式";
    }
    
    // 先连接到数据端口（在发送 LIST 之前）
    dataSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(dataSocket < 0) {
        return "无法创建数据套接字";
    }
    
    struct sockaddr_in dataAddr;
    memset(&dataAddr, 0, sizeof(dataAddr));
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_port = htons(pasvPort);
    inet_pton(AF_INET, pasvHost.c_str(), &dataAddr.sin_addr);
    
    // 连接到服务器告知的 PASV 端口
    if(::connect(dataSocket, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) < 0) {
        close(dataSocket);
        dataSocket = -1;
        return "无法连接到数据端口 " + pasvHost + ":" + std::to_string(pasvPort);
    }
    
    // 发送 LIST 命令
    string cmd = "LIST\r\n";
    send(clientSocket, cmd.c_str(), cmd.length(), 0);
    
    // 读取 150 响应
    char buffer[SIZE_BUFFER];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    string response = "";
    if(bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        response = buffer;
        // 移除末尾的\r\n
        while(!response.empty() && (response.back() == '\r' || response.back() == '\n')) {
            response.pop_back();
        }
    }
    
    // 检查响应码（150 或 125 都表示准备发送）
    if(response.substr(0, 3) != "150" && response.substr(0, 3) != "125") {
        close(dataSocket);
        dataSocket = -1;
        return "LIST 命令失败：" + response;
    }
    
    // 接收目录列表
    string result = "";
    while(true) {
        memset(buffer, 0, sizeof(buffer));
        bytesReceived = recv(dataSocket, buffer, sizeof(buffer) - 1, 0);
        if(bytesReceived <= 0) break;
        
        buffer[bytesReceived] = '\0';
        result += buffer;
    }
    
    // 关闭数据连接
    close(dataSocket);
    dataSocket = -1;
    
    // 读取 226 完成响应（重要！防止响应堆积）
    memset(buffer, 0, sizeof(buffer));
    bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if(bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        string finalResp = buffer;
        // 可选：打印最终响应
    }
    
    // 清理 pasvPort 状态
    pasvPort = -1;
    
    return result;
}

bool Client::downloadFile(const string& filename) {
    if(!pasvMode()) {
        return false;
    }
    
    dataSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(dataSocket < 0) {
        return false;
    }
    
    struct sockaddr_in dataAddr;
    memset(&dataAddr, 0, sizeof(dataAddr));
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_port = htons(pasvPort);
    inet_pton(AF_INET, pasvHost.c_str(), &dataAddr.sin_addr);
    
    if(::connect(dataSocket, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) < 0) {
        close(dataSocket);
        dataSocket = -1;
        cout << "无法连接到数据端口 " << pasvHost << ":" << pasvPort << '\n';
        return false;
    }
    
    // 发送 RETR 命令
    string cmd = "RETR " + filename + "\r\n";
    send(clientSocket, cmd.c_str(), cmd.length(), 0);
    
    // 读取 150 响应
    char buffer[SIZE_BUFFER];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    string response = "";
    if(bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        response = buffer;
        while(!response.empty() && (response.back() == '\r' || response.back() == '\n')) {
            response.pop_back();
        }
    }
    
    // 检查响应码
    if(response.substr(0, 3) != "150") {
        cout << RED << "✗ RETR 命令失败：" << response << RESET << '\n';
        close(dataSocket);
        dataSocket = -1;
        pasvPort = -1;
        return false;
    }
    
    // 接收文件内容
    string content = "";
    while(true) {
        memset(buffer, 0, sizeof(buffer));
        bytesReceived = recv(dataSocket, buffer, sizeof(buffer), 0);
        if(bytesReceived <= 0) break;
        
        content.append(buffer, bytesReceived);
    }
    
    // 关闭数据连接
    close(dataSocket);
    dataSocket = -1;

    memset(buffer, 0, sizeof(buffer));
    bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if(bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        // 可选：打印最终响应
    }
    
    // 清理 pasvPort 状态
    pasvPort = -1;
    
    // 保存文件到当前目录
    std::ofstream file(filename, std::ios::binary);
    if(!file.is_open()) {
        cout << RED << "✗ 无法创建文件：" << filename << RESET << '\n';
        return false;
    }
    
    file.write(content.c_str(), content.length());
    file.close();
    
    cout << GREEN << "✓ 文件已下载：" << RESET << filename << " (" << content.length() << " 字节)\n";
    return true;
}

// 【新增】支持断点续传的上传功能
bool Client::uploadFile(const string& filename, off_t offset) {
    // 读取本地文件
    std::ifstream file(filename, std::ios::binary);
    if(!file.is_open()) {
        cout << RED << "✗ 无法打开文件：" << filename << RESET << '\n';
        return false;
    }
    
    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    
    // 【修复】seekg 到正确位置（offset=0 时也要回到开头）
    if(offset > 0) {
        if(offset >= fileSize) {
            cout << YELLOW << "⚠ 文件已完整，无需续传" << RESET << '\n';
            file.close();
            return true;
        }
        file.seekg(offset, std::ios::beg);
    } else {
        // 【关键修复】offset=0 时必须回到文件开头，否则读取内容为空
        file.seekg(0, std::ios::beg);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    file.close();
    
    // 【新增】如果 offset > 0，先发送 REST 命令
    if(offset > 0) {
        string restResp = sendFtpCommand("REST " + std::to_string(offset));
        cout << "  " << restResp << '\n';
        if(restResp.find("350") == string::npos) {
            cout << RED << "✗ REST 命令失败" << RESET << '\n';
            return false;
        }
    }
    
    // 先进入被动模式
    if(!pasvMode()) {
        return false;
    }
    
    dataSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(dataSocket < 0) {
        return false;
    }
    
    struct sockaddr_in dataAddr;
    memset(&dataAddr, 0, sizeof(dataAddr));
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_port = htons(pasvPort);
    inet_pton(AF_INET, pasvHost.c_str(), &dataAddr.sin_addr);
    
    if(::connect(dataSocket, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) < 0) {
        close(dataSocket);
        dataSocket = -1;
        cout << "无法连接到数据端口 " << pasvHost << ":" << pasvPort << '\n';
        pasvPort = -1;
        return false;
    }
    
    // 发送 STOR 命令
    string cmd = "STOR " + filename + "\r\n";
    send(clientSocket, cmd.c_str(), cmd.length(), 0);
    
    // 读取 150 响应
    char respBuffer[SIZE_BUFFER];
    memset(respBuffer, 0, sizeof(respBuffer));
    int bytesReceived = recv(clientSocket, respBuffer, sizeof(respBuffer) - 1, 0);
    string response = "";
    if(bytesReceived > 0) {
        respBuffer[bytesReceived] = '\0';
        response = respBuffer;
        while(!response.empty() && (response.back() == '\r' || response.back() == '\n')) {
            response.pop_back();
        }
    }
    
    // 检查响应码
    if(response.substr(0, 3) != "150") {
        cout << RED << "✗ STOR 命令失败：" << response << RESET << '\n';
        close(dataSocket);
        dataSocket = -1;
        pasvPort = -1;
        return false;
    }
    
    // 发送文件内容，带进度显示
    cout << BLUE << "→ 正在上传..." << RESET << '\n';
    size_t totalSent = 0;
    size_t startOffset = offset;
    
    while(totalSent < content.length()) {
        size_t chunkSize = std::min(content.length() - totalSent, (size_t)4096);
        ssize_t sent = send(dataSocket, content.c_str() + totalSent, chunkSize, 0);
        if(sent < 0) {
            cout << RED << "✗ 发送数据失败" << RESET << '\n';
            close(dataSocket);
            dataSocket = -1;
            pasvPort = -1;
            return false;
        }
        totalSent += sent;
        
        // 显示进度
        std::streamsize currentPos = startOffset + totalSent;
        int percent = (fileSize > 0) ? (currentPos * 100 / fileSize) : 0;
        cout << "\r  进度：" << std::setw(3) << percent << "% " 
             << "(" << currentPos << "/" << fileSize << " 字节)" << std::flush;
    }
    
    cout << '\n' << GREEN << "✓ 已发送 " << (startOffset + totalSent) << " 字节" << RESET << '\n';
    
    // 关闭数据连接
    close(dataSocket);
    dataSocket = -1;
    
    // 读取 226 完成响应
    memset(respBuffer, 0, sizeof(respBuffer));
    bytesReceived = recv(clientSocket, respBuffer, sizeof(respBuffer) - 1, 0);
    if(bytesReceived > 0) {
        respBuffer[bytesReceived] = '\0';
        string finalResp = respBuffer;
        while(!finalResp.empty() && (finalResp.back() == '\r' || finalResp.back() == '\n')) {
            finalResp.pop_back();
        }
        cout << "  " << finalResp << '\n';
    }
    
    // 清理 pasvPort 状态
    pasvPort = -1;
    
    cout << GREEN << "✓ 文件已上传：" << filename << RESET << '\n';
    return true;
}

// 【修复】支持本地完整路径的上传函数
bool Client::uploadFileWithLocalPath(const string& localPath, const string& remoteFilename, off_t offset) {
    // 读取本地文件（使用完整路径）
    std::ifstream file(localPath, std::ios::binary);
    if(!file.is_open()) {
        cout << RED << "✗ 无法打开文件：" << localPath << RESET << '\n';
        return false;
    }
    
    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    
    // 【修复】seekg 到正确位置（offset=0 时也要回到开头）
    if(offset > 0) {
        if(offset >= fileSize) {
            cout << YELLOW << "⚠ 文件已完整，无需续传" << RESET << '\n';
            file.close();
            return true;
        }
        file.seekg(offset, std::ios::beg);
    } else {
        // 【关键修复】offset=0 时必须回到文件开头，否则读取内容为空
        file.seekg(0, std::ios::beg);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    file.close();
    
    // 【新增】如果 offset > 0，先发送 REST 命令
    if(offset > 0) {
        string restResp = sendFtpCommand("REST " + std::to_string(offset));
        cout << "  " << restResp << '\n';
        if(restResp.find("350") == string::npos) {
            cout << RED << "✗ REST 命令失败" << RESET << '\n';
            return false;
        }
    }
    
    // 先进入被动模式
    if(!pasvMode()) {
        return false;
    }
    
    dataSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(dataSocket < 0) {
        return false;
    }
    
    struct sockaddr_in dataAddr;
    memset(&dataAddr, 0, sizeof(dataAddr));
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_port = htons(pasvPort);
    inet_pton(AF_INET, pasvHost.c_str(), &dataAddr.sin_addr);
    
    if(::connect(dataSocket, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) < 0) {
        close(dataSocket);
        dataSocket = -1;
        cout << "无法连接到数据端口 " << pasvHost << ":" << pasvPort << '\n';
        pasvPort = -1;
        return false;
    }
    
    // 发送 STOR 命令（使用远程文件名）
    string cmd = "STOR " + remoteFilename + "\r\n";
    send(clientSocket, cmd.c_str(), cmd.length(), 0);
    
    // 读取 150 响应
    char respBuffer[SIZE_BUFFER];
    memset(respBuffer, 0, sizeof(respBuffer));
    int bytesReceived = recv(clientSocket, respBuffer, sizeof(respBuffer) - 1, 0);
    string response = "";
    if(bytesReceived > 0) {
        respBuffer[bytesReceived] = '\0';
        response = respBuffer;
        while(!response.empty() && (response.back() == '\r' || response.back() == '\n')) {
            response.pop_back();
        }
    }
    
    // 检查响应码
    if(response.substr(0, 3) != "150") {
        cout << RED << "✗ STOR 命令失败：" << response << RESET << '\n';
        close(dataSocket);
        dataSocket = -1;
        pasvPort = -1;
        return false;
    }
    
    // 发送文件内容，带进度显示
    cout << BLUE << "→ 正在上传..." << RESET << '\n';
    size_t totalSent = 0;
    size_t startOffset = offset;
    
    while(totalSent < content.length()) {
        size_t chunkSize = std::min(content.length() - totalSent, (size_t)4096);
        ssize_t sent = send(dataSocket, content.c_str() + totalSent, chunkSize, 0);
        if(sent < 0) {
            cout << RED << "✗ 发送数据失败" << RESET << '\n';
            close(dataSocket);
            dataSocket = -1;
            pasvPort = -1;
            return false;
        }
        totalSent += sent;
        
        // 显示进度
        std::streamsize currentPos = startOffset + totalSent;
        int percent = (fileSize > 0) ? (currentPos * 100 / fileSize) : 0;
        cout << "\r  进度：" << std::setw(3) << percent << "% " 
             << "(" << currentPos << "/" << fileSize << " 字节)" << std::flush;
    }
    
    cout << '\n' << GREEN << "✓ 已发送 " << (startOffset + totalSent) << " 字节" << RESET << '\n';
    
    // 关闭数据连接
    close(dataSocket);
    dataSocket = -1;
    
    // 读取 226 完成响应
    memset(respBuffer, 0, sizeof(respBuffer));
    bytesReceived = recv(clientSocket, respBuffer, sizeof(respBuffer) - 1, 0);
    if(bytesReceived > 0) {
        respBuffer[bytesReceived] = '\0';
        string finalResp = respBuffer;
        while(!finalResp.empty() && (finalResp.back() == '\r' || finalResp.back() == '\n')) {
            finalResp.pop_back();
        }
        cout << "  " << finalResp << '\n';
    }
    
    // 清理 pasvPort 状态
    pasvPort = -1;
    
    cout << GREEN << "✓ 文件已上传：" << remoteFilename << RESET << '\n';
    return true;
}

void Client::closeDataConnection() {
    if(dataSocket >= 0) {
        close(dataSocket);
        dataSocket = -1;
    }
}

// 【新增】更新当前工作目录
void Client::updateCurrentDir() {
    string resp = sendFtpCommand("PWD");
    // 【修复】添加响应检查，防止解析错误导致崩溃
    // 响应格式：257 "/path" 是当前目录
    if(resp.empty() || resp.find("530") != string::npos) {
        // 未认证或错误响应，使用默认根目录
        currentDir = "/";
        return;
    }
    size_t start = resp.find('"');
    size_t end = resp.rfind('"');
    if(start != string::npos && end != string::npos && end > start) {
        currentDir = resp.substr(start + 1, end - start - 1);
    } else {
        currentDir = "/";
    }
}

// 【新增】TUI 主菜单
void Client::showMainMenu() {
    cout << '\n' << BOLD << "╔════════════════════════════════════════╗" << RESET << '\n';
    cout << BOLD << "║     FTP TUI 客户端 - 支持断点续传      ║" << RESET << '\n';
    cout << BOLD << "╚════════════════════════════════════════╝" << RESET << '\n';
    
    // 【新增】显示当前目录
    cout << BOLD << CYAN << " 当前目录：" << RESET << currentDir << '\n';
    
    if(!g_pendingFiles.empty()) {
        cout << BOLD << RED << "\n可续传的文件 (" << g_pendingFiles.size() << " 个):" << RESET << '\n';
        for(size_t i = 0; i < g_pendingFiles.size(); i++) {
            cout << "  " << GREEN << (i+1) << ". " << RESET << g_pendingFiles[i].filename 
                 << " (已上传：" << g_pendingFiles[i].uploadedBytes << " 字节)" << '\n';
        }
    }
    
    cout << BOLD << "\n命令菜单:" << RESET << '\n';
    cout << "  [1] " << GREEN << "上传文件" << RESET << " (支持断点续传)" << '\n';
    cout << "  [2] " << YELLOW << "下载文件" << RESET << '\n';
    cout << "  [3] " << CYAN << "列出目录" << RESET << '\n';
    cout << "  [4] " << BLUE << "切换目录" << RESET << '\n';
    cout << "  [5] " << MAGENTA << "查看文件哈希" << RESET << '\n';
    cout << "  [6] " << WHITE << "刷新续传列表" << RESET << '\n';
    cout << "  [0] " << RED << "退出" << RESET << '\n';
    cout << '\n';
}

// 【新增】用户登录界面
bool Client::login() {
    cout << '\n' << BOLD << "=== 用户登录 ===" << RESET << '\n';
    cout << YELLOW << "提示：默认用户 user1/pass1, user2/pass2, admin/admin" << RESET << '\n';
    
    cout << "用户名：";
    string user;
    getline(cin, user);
    
    if(user.empty()) {
        cout << RED << "✗ 用户名不能为空" << RESET << '\n';
        return false;
    }
    
    string pass = getHiddenInput("密  码：");
    
    // 发送 USER 命令
    string resp = sendFtpCommand("USER " + user);
    cout << "  服务器响应：" << resp << '\n';
    
    // 331 表示需要密码，230 表示直接登录成功（匿名）
    if(resp.find("331") != string::npos) {
        // 发送 PASS 命令
        resp = sendFtpCommand("PASS " + pass);
        cout << "  服务器响应：" << resp << '\n';
        
        if(resp.find("230") != string::npos) {
            cout << GREEN << "✓ 登录成功！" << RESET << '\n';
            return true;
        } else if(resp.find("530") != string::npos) {
            cout << RED << "✗ 密码错误或用户不存在" << RESET << '\n';
            return false;
        } else {
            cout << RED << "✗ 登录失败：" << resp << RESET << '\n';
            return false;
        }
    } else if(resp.find("230") != string::npos) {
        cout << GREEN << "✓ 登录成功！" << RESET << '\n';
        return true;
    } else if(resp.find("530") != string::npos) {
        cout << RED << "✗ 用户不存在" << RESET << '\n';
        return false;
    } else {
        cout << RED << "✗ 登录失败：" << resp << RESET << '\n';
        return false;
    }
}

// 【新增】上传文件（带续传选择）
void Client::tuiUploadFile() {
    cout << '\n' << "请输入要上传的文件路径：";
    string filePath;
    getline(cin, filePath);
    
    if(filePath.empty()) {
        cout << RED << "✗ 文件路径不能为空" << RESET << '\n';
        return;
    }
    
    // 检查文件是否存在
    std::ifstream file(filePath, std::ios::binary);
    if(!file.is_open()) {
        cout << RED << "✗ 文件不存在或无法打开" << RESET << '\n';
        return;
    }
    file.close();
    
    // 获取文件名（用于服务器端存储）
    string filename = filePath.substr(filePath.find_last_of("/\\") + 1);
    
    // 检查是否有未完成的同名文件
    PendingFile* pf = findPendingFile(filename);
    
    if(pf != nullptr) {
        cout << YELLOW << "\n⚠ 发现未完成的文件：" << RESET << filename << '\n';
        cout << "  已上传：" << pf->uploadedBytes << " 字节" << '\n';
        cout << "  MD5: " << pf->hash << "...\n";
        cout << '\n';
        cout << "请选择操作：" << '\n';
        cout << "  [1] " << GREEN << "断点续传" << RESET << " - 从 " << pf->uploadedBytes << " 字节处继续\n";
        cout << "  [2] " << YELLOW << "重新上传" << RESET << " - 从头开始\n";
        cout << "  [0] " << RED << "取消" << RESET << '\n';
        cout << "选项：";
        
        string choice;
        getline(cin, choice);
        
        if(choice == "1") {
            cout << BLUE << "\n→ 开始断点续传..." << RESET << '\n';
            // 【修复】上传时使用完整路径读取文件，但用文件名存储到服务器
            uploadFileWithLocalPath(filePath, filename, pf->uploadedBytes);
        } else if(choice == "2") {
            cout << BLUE << "\n→ 重新上传..." << RESET << '\n';
            uploadFileWithLocalPath(filePath, filename, 0);
        } else {
            cout << "已取消\n";
            return;
        }
    } else {
        cout << BLUE << "\n→ 开始上传..." << RESET << '\n';
        uploadFileWithLocalPath(filePath, filename, 0);
    }
    
    // 刷新续传列表
    cout << BLUE << "\n→ 刷新续传列表..." << RESET << '\n';
    fetchPendingFiles();
}

// 【新增】TUI 主循环
void Client::tuiMainLoop() {
    // 登录
    if(!login()) {
        cout << RED << "登录失败，退出程序" << RESET << '\n';
        return;
    }
    
    // 【新增】获取初始工作目录
    cout << BLUE << "\n→ 获取当前目录..." << RESET << '\n';
    updateCurrentDir();
    cout << CYAN << "  当前目录：" << currentDir << RESET << '\n';
    
    // 获取初始续传列表
    cout << BLUE << "\n→ 获取续传列表..." << RESET << '\n';
    fetchPendingFiles();
    
    while(true) {
        showMainMenu();
        cout << "请输入选项：";
        
        string choice;
        getline(cin, choice);
        
        if(choice == "1") {
            tuiUploadFile();
        } else if(choice == "2") {
            cout << '\n' << "请输入要下载的文件名：";
            string filename;
            getline(cin, filename);
            if(!filename.empty()) {
                downloadFile(filename);
            }
        } else if(choice == "3") {
            cout << BLUE << "\n→ 列出目录..." << RESET << '\n';
            string result = listFiles();
            if(result.empty()) {
                cout << "目录为空\n";
            } else {
                cout << result << '\n';
            }
        } else if(choice == "4") {
            cout << '\n' << "请输入目录路径：";
            string path;
            getline(cin, path);
            if(!path.empty()) {
                string resp = sendFtpCommand("CWD " + path);
                cout << "  " << resp << '\n';
                // 【新增】目录切换后更新当前目录
                updateCurrentDir();
            }
        } else if(choice == "5") {
            cout << '\n' << "请输入文件名：";
            string filename;
            getline(cin, filename);
            if(!filename.empty()) {
                cout << BLUE << "\n→ 计算文件哈希..." << RESET << '\n';
                string resp = sendFtpCommand("HASH " + filename);
                cout << "  " << resp << '\n';
            }
        } else if(choice == "6") {
            cout << BLUE << "\n→ 刷新续传列表..." << RESET << '\n';
            fetchPendingFiles();
            cout << GREEN << "✓ 已刷新" << RESET << '\n';
        } else if(choice == "0") {
            sendFtpCommand("QUIT");
            cout << GREEN << "\n再见！" << RESET << '\n';
            break;
        } else {
            cout << YELLOW << "无效选项，请重新输入" << RESET << '\n';
        }
    }
}

void Client::microShell() {
    string command;

    cout << "\n╔════════════════════════════════════════╗\n";
    cout << "║       欢迎使用简易 FTP 客户端            ║\n";
    cout << "╠════════════════════════════════════════╣\n";
    cout << "║ 支持命令：                              ║\n";
    cout << "║  list              - 列出远程文件       ║\n";
    cout << "║  get <文件名>       - 下载文件          ║\n";
    cout << "║  put <文件名>       - 上传文件          ║\n";
    cout << "║  cd <目录>          - 切换目录          ║\n";
    cout << "║  pwd               - 显示当前目录       ║\n";
    cout << "║  help              - 显示帮助           ║\n";
    cout << "║  quit              - 退出               ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";

    while (true) {
        cout << "ftp> ";
        std::getline(cin, command);
        
        if (command.empty()) {
            continue;
        }
        
        // 解析本地命令
        if(command == "quit" || command == "exit") {
            sendFtpCommand("QUIT");
            cout << "已断开连接\n";
            break;
        } else if(command == "list" || command == "ls") {
            string result = listFiles();
            if(result.empty()) {
                cout << "目录为空\n";
            } else {
                cout << result << '\n';
            }
        } else if(command.substr(0, 4) == "get ") {
            string filename = command.substr(4);
            if(!downloadFile(filename)) {
                cout << "下载失败：" << filename << "\n";
            }
        } else if(command.substr(0, 4) == "put ") {
            string filename = command.substr(4);
            if(!uploadFile(filename, 0)) {
                cout << "上传失败：" << filename << "\n";
            }
        } else if(command.substr(0, 3) == "cd ") {
            string dirname = command.substr(3);
            sendFtpCommand("CWD " + dirname);
        } else if(command == "cdup" || command == "cd ..") {
            sendFtpCommand("CDUP");
        } else if(command == "pwd") {
            sendFtpCommand("PWD");
        } else if(command == "help" || command == "?") {
            sendFtpCommand("HELP");
        } else if(command == "pasv") {
            pasvMode();
        } else {
            // 直接发送 FTP 命令
            sendFtpCommand(command);
        }
    }
}

void Client::disconnect() {
    closeDataConnection();
    
    if(clientSocket != -1) {
        sendFtpCommand("QUIT");
        close(clientSocket);
        clientSocket = -1;
        cout << "已关闭链接\n";
    }
}

bool Client::connectServer() {
    return connectFtpServer();
}

string Client::sendCommand(const string& command) {
    return sendFtpCommand(command);
}
