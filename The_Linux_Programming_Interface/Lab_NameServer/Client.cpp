#include "Client.h"
#include <sstream> 


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

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::runtime_error("无法连接到服务器");
        close(clientSocket);
        return false;
    }

    cout << "已连接到 FTP 服务器 " << serverAddress << ":" << serverPort << '\n';

    // 接收欢迎消息
    receiveResponse();
    
    sendFtpCommand("USER anonymous");
    sendFtpCommand("PASS anonymous@");
    
    return true;
}

void Client::receiveResponse() {
    char buffer[SIZE_BUFFER];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    if(bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        cout << buffer;
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
        cout << response << '\n';
        return response;
    }
    
    return "链接已被服务器关闭";
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
    if(!pasvMode()) {
        return "无法进入被动模式";
    }
    
    // 发送 LIST 命令
    string response = sendFtpCommand("LIST");
    
    // 检查响应码
    if(response.substr(0, 3) != "150") {
        return "LIST 命令失败：" + response;
    }
    
    // 连接到数据端口
    dataSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(dataSocket < 0) {
        return "无法创建数据套接字";
    }
    
    struct sockaddr_in dataAddr;
    memset(&dataAddr, 0, sizeof(dataAddr));
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_port = htons(pasvPort);
    inet_pton(AF_INET, pasvHost.c_str(), &dataAddr.sin_addr);
    
    if(connect(dataSocket, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) < 0) {
        close(dataSocket);
        dataSocket = -1;
        return "无法连接到数据端口";
    }
    
    // 接收目录列表
    string result = "";
    char buffer[SIZE_BUFFER];
    while(true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(dataSocket, buffer, sizeof(buffer) - 1, 0);
        if(bytesReceived <= 0) break;
        
        buffer[bytesReceived] = '\0';
        result += buffer;
    }
    
    // 关闭数据连接
    closeDataConnection();
    
    return result;
}

bool Client::downloadFile(const string& filename) {
    // 先进入被动模式
    if(!pasvMode()) {
        return false;
    }
    
    // 发送 RETR 命令
    string response = sendFtpCommand("RETR " + filename);
    
    // 检查响应码
    if(response.substr(0, 3) != "150") {
        cout << "RETR 命令失败：" + response << '\n';
        return false;
    }
    
    // 连接到数据端口
    dataSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(dataSocket < 0) {
        return false;
    }
    
    struct sockaddr_in dataAddr;
    memset(&dataAddr, 0, sizeof(dataAddr));
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_port = htons(pasvPort);
    inet_pton(AF_INET, pasvHost.c_str(), &dataAddr.sin_addr);
    
    if(connect(dataSocket, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) < 0) {
        close(dataSocket);
        dataSocket = -1;
        return false;
    }
    
    // 接收文件内容
    string content = "";
    char buffer[SIZE_BUFFER];
    while(true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(dataSocket, buffer, sizeof(buffer), 0);
        if(bytesReceived <= 0) break;
        
        content.append(buffer, bytesReceived);
    }
    
    // 关闭数据连接
    closeDataConnection();
    
    // 保存文件到当前目录
    std::ofstream file(filename, std::ios::binary);
    if(!file.is_open()) {
        cout << "无法创建文件：" << filename << '\n';
        return false;
    }
    
    file.write(content.c_str(), content.length());
    file.close();
    
    cout << "文件已下载：" << filename << " (" << content.length() << " 字节)\n";
    return true;
}

// 新增：上传文件
bool Client::uploadFile(const string& filename) {
    // 读取本地文件
    std::ifstream file(filename, std::ios::binary);
    if(!file.is_open()) {
        cout << "无法打开文件：" << filename << '\n';
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    file.close();
    
    // 先进入被动模式
    if(!pasvMode()) {
        return false;
    }
    
    // 发送 STOR 命令
    string response = sendFtpCommand("STOR " + filename);
    
    // 检查响应码
    if(response.substr(0, 3) != "150") {
        cout << "STOR 命令失败：" + response << '\n';
        return false;
    }
    
    // 连接到数据端口
    dataSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(dataSocket < 0) {
        return false;
    }
    
    struct sockaddr_in dataAddr;
    memset(&dataAddr, 0, sizeof(dataAddr));
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_port = htons(pasvPort);
    inet_pton(AF_INET, pasvHost.c_str(), &dataAddr.sin_addr);
    
    if(connect(dataSocket, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) < 0) {
        close(dataSocket);
        dataSocket = -1;
        return false;
    }
    
    // 发送文件内容
    size_t totalSent = 0;
    while(totalSent < content.length()) {
        ssize_t sent = send(dataSocket, content.c_str() + totalSent, 
                           content.length() - totalSent, 0);
        if(sent < 0) {
            cout << "发送数据失败\n";
            close(dataSocket);
            dataSocket = -1;
            return false;
        }
        totalSent += sent;
    }
    
    cout << "已发送 " << totalSent << " 字节\n";
    
    // 关闭数据连接
    closeDataConnection();
    
    cout << "文件已上传：" << filename << '\n';
    return true;
}

// 新增：关闭数据连接
void Client::closeDataConnection() {
    if(dataSocket >= 0) {
        close(dataSocket);
        dataSocket = -1;
        pasvPort = -1;
    }
}

// 新增：FTP 交互模式
void Client::microShell() {
    string command;

    cout << "\nFTP 支持命令:\n";
    cout << "  list              - 列出远程文件\n";
    cout << "  get <filename>    - 下载文件\n";
    cout << "  put <filename>    - 上传文件\n";
    cout << "  pasv              - 进入被动模式\n";
    cout << "  quit              - 退出\n\n";

    while (true) {
        cout << "ftp> ";
        std::getline(cin, command);
        
        if (command.empty()) {
            continue;
        }
        
        // 解析本地命令
        if(command == "quit" || command == "exit") {
            sendFtpCommand("QUIT");
            break;
        } else if(command == "list") {
            string result = listFiles();
            cout << result << '\n';
        } else if(command.substr(0, 4) == "get ") {
            string filename = command.substr(4);
            downloadFile(filename);
        } else if(command.substr(0, 4) == "put ") {
            string filename = command.substr(4);
            uploadFile(filename);
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
