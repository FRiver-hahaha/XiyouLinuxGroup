#include "Server.h"
#include <arpa/inet.h>

// 注释掉原构造函数
// Server::Server(int port) 
//     : serverSocket(-1), port(port) {
//     signal(SIGPIPE, SIG_IGN);
//     threadPool = std::make_unique<ThreadPool>(4);
// }

// 新增：FTP 服务器构造函数，初始化控制端口和数据端口
Server::Server(int controlPort, int dataPort) 
    : controlServerSocket(-1), dataServerSocket(-1), epollFd(-1),
      controlPort(controlPort), dataPort(dataPort), nextPasvPort(PASV_PORT_BASE) {
    signal(SIGPIPE, SIG_IGN);  // 忽略 SIGPIPE 信号，防止发送时崩溃
    threadPool = std::make_unique<ThreadPool>(4);
    serverIp = getServerIp();  // 获取服务器 IP 地址
}

/*
    服务端监听部分：

    共分为三个部分，控制信息，数据传输，epoll监听处理
    控制信息和数据传输的创建思路基本一致，
    通过socket,bind,listen创建socket，监听客户端的链接，
    
    epoll监听处理使用：epoll_create1,epoll_ctl处理监听
    
*/

bool Server::start() {
    // 控制连接端口部分
    controlServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(controlServerSocket < 0) {
        log("无法创建控制套接字");
        return false;
    }

    int optval = 1;
    setsockopt(controlServerSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    setsockopt(controlServerSocket, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));

    struct sockaddr_in controlAddr;
    memset(&controlAddr, 0, sizeof(controlAddr));
    controlAddr.sin_family = AF_INET;
    controlAddr.sin_port = htons(controlPort);  // 控制端口 2100
    controlAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(controlServerSocket, (struct sockaddr*)&controlAddr, sizeof(controlAddr)) < 0) {
        close(controlServerSocket);
        log("无法绑定控制套接字");
        return false;
    }

    if(listen(controlServerSocket, SOMAXCONN) < 0) {
        close(controlServerSocket);
        log("无法监听控制端口");
        return false;
    }

    // 数据传输端口部分
    dataServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(dataServerSocket < 0) {
        log("无法创建数据套接字");
        close(controlServerSocket);
        return false;
    }

    setsockopt(dataServerSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in dataAddr;
    memset(&dataAddr, 0, sizeof(dataAddr));
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_port = htons(dataPort);  // 2000
    dataAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(dataServerSocket, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) < 0) {
        close(dataServerSocket);
        log("无法绑定数据套接字");
    } else {
        if(listen(dataServerSocket, SOMAXCONN) < 0) {
            close(dataServerSocket);
            dataServerSocket = -1;
            log("无法监听数据端口");
        }
    }

    // epoll 监听部分
    epollFd = epoll_create1(0);
    if(epollFd < 0) {
        log("无法创建 epoll");
        close(controlServerSocket);
        if(dataServerSocket >= 0) close(dataServerSocket);
        return false;
    }

    // 将控制套接字添加到 epoll 监听
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = controlServerSocket;
    if(epoll_ctl(epollFd, EPOLL_CTL_ADD, controlServerSocket, &ev) < 0) {
        log("无法添加控制套接字到 epoll");
        close(epollFd);
        close(controlServerSocket);
        if(dataServerSocket >= 0) close(dataServerSocket);
        return false;
    }
    // 输出服务端当前状态信息
    printServerInfo();
    return true;
}

/*
    服务端执行部分：
    使用FtpSession包装数据
    创建一个epoll结构体，存储epoll
    执行epoll_wait,进入epoll监听状态
    遇到客户端的链接就建立链接
    之后使用线程池处理客户数据
*/

void Server::run() {
    struct epoll_event events[MAX_EPOLL_EVENTS];
    log("等待客户端链接...");

    while(running) {
        // epoll_wait 等待事件，超时 1 秒
        int nfds = epoll_wait(epollFd, events, MAX_EPOLL_EVENTS, 1000);
        
        if(!running) break;
        
        if(nfds < 0) {
            if(errno == EINTR) continue;  // 被信号中断，继续
            log("epoll_wait 错误");
            break;
        }

        // 处理所有就绪的事件
        for(int i = 0; i < nfds; i++) {
            if(events[i].data.fd == controlServerSocket) {
                // 控制套接字有新连接
                struct sockaddr_in clientAddr;
                socklen_t clientLen = sizeof(clientAddr);
                int clientSock = accept(controlServerSocket, (struct sockaddr*)&clientAddr, &clientLen);
                
                if(clientSock < 0) {
                    log("接受控制连接失败");
                    continue;
                }
                
                char clientIp[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
                int clientPort = ntohs(clientAddr.sin_port);
                
                string logMsg = "新客户端连接：" + string(clientIp) + ":" + std::to_string(clientPort);
                log(logMsg);
                
                
                FtpSession* session = new FtpSession();
                session->controlSock = clientSock;
                session->clientIp = clientIp;
                
                {
                    std::lock_guard<mutex> lg(sessionMutex);
                    sessions[clientSock] = session;
                }
                
                string welcomeMsg = "FTP 服务器就绪\r\n";
                send(clientSock, welcomeMsg.c_str(), welcomeMsg.length(), 0);
                
                // 将会话套接字添加到 epoll 监听
                struct epoll_event clientEv;
                clientEv.events = EPOLLIN | EPOLLET;  // 边缘触发模式
                clientEv.data.fd = clientSock;
                epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSock, &clientEv);
                
            } else if(events[i].data.fd >= 0) {
                int clientSock = events[i].data.fd;
                // 线程池处理数据
                threadPool->submit(
                    [this, clientSock]() {
                        handleClientData(clientSock);
                    },
                    []() {}
                );
            }
        }
    }

    log("服务器已关闭链接");
    
    // 清理资源
    if(epollFd >= 0) close(epollFd);
    if(controlServerSocket >= 0) close(controlServerSocket);
    if(dataServerSocket >= 0) close(dataServerSocket);
    
    log("已关闭服务器");
}

/*
    处理数据部分：
    使用recv接受数据
    如果接受成功，则进入处理数据部分
*/

void Server::handleClientData(int clientSock) {
    char buffer[SIZE_BUFFER];
    string command;
    
    // 接收数据
    size_t bytesReceived = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    
    if(bytesReceived <= 0) {
        if(bytesReceived == 0) {
            log("客户端主动关闭连接");
        } else {
            log("recv 错误");
        }
        removeSession(clientSock);
        return;
    }
    
    buffer[bytesReceived] = '\0';
    command = buffer;
    
    // 移除末尾的\r\n
    while(!command.empty() && (command.back() == '\r' || command.back() == '\n')) {
        command.pop_back();
    }
    
    if(command.empty()) {
        return;
    }
    
    string logMsg = "接收到 FTP 命令：" + command;
    log(logMsg);
    

    handleFtpCommand(clientSock, command);
}

/*
    解析数据部分：
    将数据拆解成命令词和参数的形式，
    之后分部分处理命令
*/

void Server::handleFtpCommand(int clientSock, const string& command) {

    string cmd;
    string args;
    
    size_t space = command.find(' ');
    if(space != string::npos) {
        cmd = command.substr(0, space);
        args = command.substr(space + 1);
    } else {
        cmd = command;
        args = "";
    }
    
    // 转换为大写
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    
    // 发送响应
    if(cmd == "PASV") {
        handlePasvCommand(clientSock);
    } else if(cmd == "LIST") {
        handleListCommand(clientSock);
    } else if(cmd == "RETR") {
        handleRetrCommand(clientSock, args);
    } else if(cmd == "STOR") {
        handleStorCommand(clientSock, args);
    } else if(cmd == "PWD") {
        string resp = "\"" + getWorkingDir(clientSock) + "\" 是当前目录\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else if(cmd == "CWD") {
        string resp = "目录已更改\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else if(cmd == "TYPE") {
        string resp = "类型设置为 " + args + "\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else if(cmd == "QUIT") {
        string resp = "再见\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
        removeSession(clientSock);
    } else if(cmd == "USER" || cmd == "PASS") {
        // 匿名登录，直接成功
        string resp = "登录成功\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else if(cmd == "SYST") {
        string resp = "UNIX Type: L8\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else if(cmd == "FEAT") {
        string resp = "功能列表\r\n PASV\r\n LIST\r\n RETR\r\n STOR\r\n211 END\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else {
        string resp = "命令未实现：" + cmd + "\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    }
}

string Server::getWorkingDir(int clientSock) {
    std::lock_guard<mutex> lg(sessionMutex);
    auto it = sessions.find(clientSock);
    if(it != sessions.end() && it->second) {
        return it->second->workingDir;
    }
    return "/";
}

void Server::handlePasvCommand(int clientSock) {
    int pasvPort = allocatePasvPort();
    
    std::lock_guard<mutex> lg(sessionMutex);
    auto it = sessions.find(clientSock);
    if(it != sessions.end() && it->second) {
        it->second->pasvPort = pasvPort;
        it->second->pasvMode = true;
    }
    
    string ip = serverIp;
    int h1, h2, h3, h4;
    sscanf(ip.c_str(), "%d.%d.%d.%d", &h1, &h2, &h3, &h4);
    
    // port = p1*256 + p2
    int p1 = pasvPort / 256;
    int p2 = pasvPort % 256;
    
    // 227 entering passive mode (h1,h2,h3,h4,p1,p2)
    string resp = "227 entering passive mode (" + 
                  std::to_string(h1) + "," + std::to_string(h2) + "," +
                  std::to_string(h3) + "," + std::to_string(h4) + "," +
                  std::to_string(p1) + "," + std::to_string(p2) + ")\r\n";
    
    log("PASV 响应：" + resp);
    send(clientSock, resp.c_str(), resp.length(), 0);
}

/*
    处理端口分配的情况
*/

int Server::allocatePasvPort() {
    std::lock_guard<mutex> lg(dataPortMutex);
    int port = nextPasvPort;
    nextPasvPort++;
    if(nextPasvPort > PASV_PORT_BASE + 100) {
        nextPasvPort = PASV_PORT_BASE;  // 循环使用端口
    }
    return port;
}

void Server::handleListCommand(int clientSock) {
    string resp = "准备发送目录列表\r\n";
    send(clientSock, resp.c_str(), resp.length(), 0);
    
    FtpSession* session = nullptr;
    {
        std::lock_guard<mutex> lg(sessionMutex);
        auto it = sessions.find(clientSock);
        if(it != sessions.end()) {
            session = it->second;
        }
    }
    
    if(!session) {
        string errResp = "无法打开数据连接\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    int dataSock = -1;
    if(session->pasvMode && session->pasvPort > 0) {
        // 创建监听套接字等待客户端连接
        int pasvListenSock = socket(AF_INET, SOCK_STREAM, 0);
        if(pasvListenSock >= 0) {
            int optval = 1;
            setsockopt(pasvListenSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
            
            struct sockaddr_in pasvAddr;
            memset(&pasvAddr, 0, sizeof(pasvAddr));
            pasvAddr.sin_family = AF_INET;
            pasvAddr.sin_port = htons(session->pasvPort);
            pasvAddr.sin_addr.s_addr = INADDR_ANY;
            
            if(bind(pasvListenSock, (struct sockaddr*)&pasvAddr, sizeof(pasvAddr)) >= 0) {
                if(listen(pasvListenSock, 1) >= 0) {
                    // 设置超时
                    struct timeval tv;
                    tv.tv_sec = 5;
                    tv.tv_usec = 0;
                    setsockopt(pasvListenSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
                    
                    struct sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
                    dataSock = accept(pasvListenSock, (struct sockaddr*)&clientAddr, &clientLen);
                }
            }
            close(pasvListenSock);
        }
    }
    
    if(dataSock < 0) {
        string errResp = "无法打开数据连接\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    string dirList = listDirectory(session->workingDir);
    
    sendDataToClient(dataSock, dirList);
    
    close(dataSock);
    session->dataSock = -1;
    
    string completeResp = "传输完成\r\n";
    send(clientSock, completeResp.c_str(), completeResp.length(), 0);
}

string Server::listDirectory(const string& path) {
    string result = "";
    string actualPath = path == "/" ? "." : path;
    
    DIR* dir = opendir(actualPath.c_str());
    if(dir == nullptr) {
        return "目录不存在或无法访问\r\n";
    }
    
    struct dirent* entry;
    while((entry = readdir(dir)) != nullptr) {
        string name = entry->d_name;
        if(name == ".") continue; 
        
        string fullPath = actualPath + "/" + name;
        struct stat statbuf;
        string typeStr = "-";
        
        if(stat(fullPath.c_str(), &statbuf) == 0) {
            if(S_ISDIR(statbuf.st_mode)) {
                typeStr = "d";  
            }
            
            
            char line[512];
            snprintf(line, sizeof(line), "%srw-r--r-- 1 ftp ftp %8ld Jan  1 00:00 %s\r\n",
                     typeStr.c_str(), (long)statbuf.st_size, name.c_str());
            result += line;
        }
    }
    
    closedir(dir);
    return result;
}

void Server::handleRetrCommand(int clientSock, const string& filename) {
    if(filename.empty()) {
        string resp = "5需要文件名参数\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
        return;
    }
    
    
    string resp = "准备发送文件 " + filename + "\r\n";
    send(clientSock, resp.c_str(), resp.length(), 0);
    
    
    FtpSession* session = nullptr;
    {
        std::lock_guard<mutex> lg(sessionMutex);
        auto it = sessions.find(clientSock);
        if(it != sessions.end()) {
            session = it->second;
        }
    }
    
    if(!session) {
        string errResp = "无法打开数据连接\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    string filePath = session->workingDir + "/" + filename;
    if(filePath[0] == '/' && filePath[1] == '/') {
        filePath = filePath.substr(1);
    }

    string content = readFile(filePath);
    if(content.empty()) {
        string errResp = "文件不存在或无法读取：" + filename + "\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    // 建立数据连接并发送文件
    int dataSock = -1;
    if(session->pasvMode && session->pasvPort > 0) {
        int pasvListenSock = socket(AF_INET, SOCK_STREAM, 0);
        if(pasvListenSock >= 0) {
            int optval = 1;
            setsockopt(pasvListenSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
            
            struct sockaddr_in pasvAddr;
            memset(&pasvAddr, 0, sizeof(pasvAddr));
            pasvAddr.sin_family = AF_INET;
            pasvAddr.sin_port = htons(session->pasvPort);
            pasvAddr.sin_addr.s_addr = INADDR_ANY;
            
            if(bind(pasvListenSock, (struct sockaddr*)&pasvAddr, sizeof(pasvAddr)) >= 0) {
                if(listen(pasvListenSock, 1) >= 0) {
                    struct timeval tv;
                    tv.tv_sec = 5;
                    tv.tv_usec = 0;
                    setsockopt(pasvListenSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
                    
                    struct sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
                    dataSock = accept(pasvListenSock, (struct sockaddr*)&clientAddr, &clientLen);
                }
            }
            close(pasvListenSock);
        }
    }
    
    if(dataSock < 0) {
        string errResp = "无法打开数据连接\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    // 发送文件内容
    sendDataToClient(dataSock, content);
    
    close(dataSock);
    session->dataSock = -1;
    
    // 发送完成响应
    string completeResp = "传输完成\r\n";
    send(clientSock, completeResp.c_str(), completeResp.length(), 0);
}

// 新增：处理 STOR 上传命令
void Server::handleStorCommand(int clientSock, const string& filename) {
    if(filename.empty()) {
        string resp = "需要文件名参数\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
        return;
    }
    
    // 发送准备接收响应
    string resp = "准备接收文件 " + filename + "\r\n";
    send(clientSock, resp.c_str(), resp.length(), 0);
    
    // 获取会话
    FtpSession* session = nullptr;
    {
        std::lock_guard<mutex> lg(sessionMutex);
        auto it = sessions.find(clientSock);
        if(it != sessions.end()) {
            session = it->second;
        }
    }
    
    if(!session) {
        string errResp = "无法打开数据连接\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    // 建立数据连接
    int dataSock = -1;
    if(session->pasvMode && session->pasvPort > 0) {
        int pasvListenSock = socket(AF_INET, SOCK_STREAM, 0);
        if(pasvListenSock >= 0) {
            int optval = 1;
            setsockopt(pasvListenSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
            
            struct sockaddr_in pasvAddr;
            memset(&pasvAddr, 0, sizeof(pasvAddr));
            pasvAddr.sin_family = AF_INET;
            pasvAddr.sin_port = htons(session->pasvPort);
            pasvAddr.sin_addr.s_addr = INADDR_ANY;
            
            if(bind(pasvListenSock, (struct sockaddr*)&pasvAddr, sizeof(pasvAddr)) >= 0) {
                if(listen(pasvListenSock, 1) >= 0) {
                    struct timeval tv;
                    tv.tv_sec = 5;
                    tv.tv_usec = 0;
                    setsockopt(pasvListenSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
                    
                    struct sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
                    dataSock = accept(pasvListenSock, (struct sockaddr*)&clientAddr, &clientLen);
                }
            }
            close(pasvListenSock);
        }
    }
    
    if(dataSock < 0) {
        string errResp = "无法打开数据连接\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    // 接收文件内容
    string content = receiveDataFromClient(dataSock, 1024 * 1024);
    
    close(dataSock);
    session->dataSock = -1;
    

    string filePath = session->workingDir + "/" + filename;
    if(filePath[0] == '/' && filePath[1] == '/') {
        filePath = filePath.substr(1);
    }

    if(saveFile(filePath, content)) {
        string completeResp = "传输完成，文件已保存\r\n";
        send(clientSock, completeResp.c_str(), completeResp.length(), 0);
    } else {
        string errResp = "无法保存文件\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
    }
}

void Server::sendDataToClient(int dataSock, const string& data) {
    size_t totalSent = 0;
    while(totalSent < data.length()) {
        ssize_t sent = send(dataSock, data.c_str() + totalSent, data.length() - totalSent, 0);
        if(sent < 0) {
            log("发送数据失败");
            break;
        }
        totalSent += sent;
    }
}

string Server::receiveDataFromClient(int dataSock, size_t maxBytes) {
    string result = "";
    char buffer[SIZE_BUFFER];
    size_t totalReceived = 0;
    
    while(totalReceived < maxBytes) {
        ssize_t received = recv(dataSock, buffer, sizeof(buffer), 0);
        if(received <= 0) {
            break;
        }
        result.append(buffer, received);
        totalReceived += received;
    }
    
    return result;
}

string Server::readFile(const string& path) {
    std::ifstream file(path, std::ios::binary);
    if(!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Server::saveFile(const string& path, const string& content) {
    std::ofstream file(path, std::ios::binary);
    if(!file.is_open()) {
        return false;
    }
    
    file.write(content.c_str(), content.length());
    file.close();
    return true;
}

void Server::removeSession(int clientSock) {
    std::lock_guard<mutex> lg(sessionMutex);
    auto it = sessions.find(clientSock);
    if(it != sessions.end()) {
        if(it->second) {
            if(it->second->dataSock >= 0) {
                close(it->second->dataSock);
            }
            if(it->second->pasvPort > 0) {
                releasePasvPort(it->second->pasvPort);
            }
            delete it->second;
        }
        
        // 从 epoll 移除
        epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSock, nullptr);
        close(clientSock);
        
        sessions.erase(it);
        
        string logMsg = "会话已移除，剩余会话数：" + std::to_string(sessions.size());
        log(logMsg);
    }
}

// 新增：获取服务器 IP
string Server::getServerIp() {
    auto ips = getLocalIps();
    if(!ips.empty()) {
        return ips[0];
    }
    return "127.0.0.1";
}

std::vector<string> Server::getLocalIps() {
    std::vector<string> ips;

    struct ifaddrs *addrs;
    if(getifaddrs(&addrs) == -1) {
        return ips;
    }

    for(struct ifaddrs *ifa = addrs; ifa != nullptr; ifa = ifa->ifa_next) {
        if(ifa->ifa_addr == nullptr) {
            continue;
        }

        if(ifa->ifa_addr->sa_family == AF_INET) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr, ip, INET_ADDRSTRLEN);
            string ipStr(ip);
            if(ipStr != "127.0.0.1") {
                ips.push_back(ipStr);
            }
        }
    }

    freeifaddrs(addrs);
    return ips;
}

void Server::printServerInfo() {
    cout << "\n╔════════════════════════════════════════╗\n";
    cout << "║     简易 FTP 服务器已启动，等待连接...   ║\n";
    cout << "╠════════════════════════════════════════╣\n";
    cout << "║ 控制端口：" << std::setw(29) << std::left << controlPort << "║\n";
    cout << "║ 数据端口：" << std::setw(29) << std::left << dataPort << "║\n";
    
    // 打印本机回环地址
    cout << "║ 本地访问：127.0.0.1:" << controlPort;
    cout << std::string(20 - std::to_string(controlPort).length(), ' ') << "║\n";
    
    // 打印所有局域网 IP
    auto ips = getLocalIps();
    for (const auto& ip : ips) {
        cout << "║ 局域网访问：" << ip << ":" << controlPort;
        int padding = 19 - ip.length() - std::to_string(controlPort).length();
        if (padding > 0) cout << std::string(padding, ' ');
        cout << "║\n";
    }
    
    cout << "╠════════════════════════════════════════╣\n";
    cout << "║ 支持命令：                              ║\n";
    cout << "║  PASV - 被动模式                        ║\n";
    cout << "║  LIST - 列出目录                        ║\n";
    cout << "║  RETR - 下载文件                        ║\n";
    cout << "║  STOR - 上传文件                        ║\n";
    cout << "║  QUIT - 退出                            ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
}

int Server::getSessionCount() {
    std::lock_guard<mutex> lg(sessionMutex);
    return sessions.size();
}

void Server::stop() {
    if(!running) return;

    running = false;
    
    // 关闭所有会话
    {
        std::lock_guard<mutex> lg(sessionMutex);
        for(auto& [sock, session] : sessions) {
            if(session) {
                string msg = "421 服务器正在关闭...\r\n";
                send(sock, msg.c_str(), msg.length(), 0);
                if(session->dataSock >= 0) {
                    close(session->dataSock);
                }
                delete session;
            }
            close(sock);
        }
        sessions.clear();
    }

    if(epollFd >= 0) {
        close(epollFd);
        epollFd = -1;
    }
    
    if(controlServerSocket >= 0) {
        close(controlServerSocket);
        controlServerSocket = -1;
    }
    
    if(dataServerSocket >= 0) {
        close(dataServerSocket);
        dataServerSocket = -1;
    }
    
    log("FTP 服务器已关闭");
}

void Server::releasePasvPort(int port) {
   
    string msg = "释放被动模式端口：" + std::to_string(port) + "\n";
    cout << msg;
}
