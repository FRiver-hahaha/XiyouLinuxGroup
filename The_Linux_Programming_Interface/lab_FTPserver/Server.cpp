#include "Server.h"
#include <arpa/inet.h>
#include <sstream>
#include <fcntl.h>
#include <sys/sendfile.h> 

Server::Server(int controlPort, int dataPort) 
    : controlServerSocket(-1), dataServerSocket(-1), epollFd(-1),
      controlPort(controlPort), dataPort(dataPort), nextPasvPort(PASV_PORT_BASE) {
    signal(SIGPIPE, SIG_IGN);  // 忽略 SIGPIPE 信号，防止发送时崩溃
    threadPool = std::make_unique<ThreadPool>(4);
    // 获取服务器 IP 地址（用于非本地客户端连接时返回）
    auto ips = getLocalIps();
    serverIp = ips.empty() ? "127.0.0.1" : ips[0];  // 优先使用局域网 IP
}

/*
    服务端监听部分：

    共分为三个部分，控制信息，数据传输，epoll 监听处理
    控制信息和数据传输的创建思路基本一致，
    通过 socket,bind,listen 创建 socket，监听客户端的链接，
    
    epoll 监听处理使用：epoll_create1,epoll_ctl 处理监听
    
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
    使用 FtpSession 包装数据
    创建一个 epoll 结构体，存储 epoll
    执行 epoll_wait，进入 epoll 监听状态
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
    使用 recv 接受数据
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
        string resp = "257 \"" + getWorkingDir(clientSock) + "\" 是当前目录\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else if(cmd == "CWD") {
        // 修改：实际处理目录切换逻辑
        handleCwdCommand(clientSock, args);
    } else if(cmd == "CDUP") {
        // 新增：切换到父目录
        handleCdupCommand(clientSock);
    } else if(cmd == "TYPE") {
        string resp = "200 类型设置为 " + args + "\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else if(cmd == "QUIT") {
        string resp = "221 再见\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
        removeSession(clientSock);
    } else if(cmd == "SHUTDOWN") {
        // 新增：优雅关闭服务器
        handleShutdownCommand(clientSock);
    } else if(cmd == "USER" || cmd == "PASS") {
        // 匿名登录，直接成功
        string resp = "230 登录成功\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else if(cmd == "SYST") {
        string resp = "215 UNIX Type: L8\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else if(cmd == "FEAT") {
        string resp = "211-功能列表\r\n PASV\r\n LIST\r\n RETR\r\n STOR\r\n PWD\r\n CWD\r\n CDUP\r\n SHUTDOWN\r\n211 END\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else if(cmd == "HELP") {
        string resp = "214-支持命令:\r\n"
                      " PASV - 进入被动模式\r\n"
                      " LIST - 列出目录\r\n"
                      " RETR <文件> - 下载文件\r\n"
                      " STOR <文件> - 上传文件\r\n"
                      " PWD - 显示当前目录\r\n"
                      " CWD <目录> - 切换目录\r\n"
                      " CDUP - 返回上级目录\r\n"
                      " SHUTDOWN - 关闭服务器\r\n"
                      " QUIT - 退出\r\n"
                      "214 END\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    } else {
        string resp = "502 命令未实现：" + cmd + "\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
    }
}

void Server::handleCwdCommand(int clientSock, const string& path) {
    if(path.empty()) {
        string resp = "需要目录参数\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
        return;
    }
    
    FtpSession* session = nullptr;
    {
        std::lock_guard<mutex> lg(sessionMutex);
        auto it = sessions.find(clientSock);
        if(it != sessions.end()) {
            session = it->second;
        }
    }
    
    if(!session) {
        string resp = "会话不存在\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
        return;
    }
    
    string newDir = session->workingDir;
    
    if(path[0] == '/') {
        // 绝对路径
        newDir = path;
    } else {
        // 相对路径
        if(newDir != "/") {
            newDir = newDir + "/" + path;
        } else {
            newDir = "/" + path;
        }
    }
    
    // 规范化路径
    std::vector<string> parts;
    std::istringstream iss(newDir);
    string part;
    while(std::getline(iss, part, '/')) {
        if(part.empty() || part == ".") {
            continue;
        } else if(part == "..") {
            if(!parts.empty()) {
                parts.pop_back();
            }
        } else {
            parts.push_back(part);
        }
    }
    
    newDir = "/";
    for(size_t i = 0; i < parts.size(); i++) {
        if(i > 0) newDir += "/";
        newDir += parts[i];
    }
    
    // 检查目录是否存在
    DIR* dir = opendir(newDir.c_str());
    if(dir == nullptr) {
        string resp = "5目录不存在：" + path + "\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
        return;
    }
    closedir(dir);
    
    // 更新工作目录
    session->workingDir = newDir;
    string resp = "250 目录已更改为 " + newDir + "\r\n";
    send(clientSock, resp.c_str(), resp.length(), 0);
}

void Server::handleCdupCommand(int clientSock) {
    handleCwdCommand(clientSock, "..");
}

void Server::handleShutdownCommand(int clientSock) {
    string resp = "221 服务器正在关闭...\r\n";
    send(clientSock, resp.c_str(), resp.length(), 0);
    
    log("收到关闭请求，正在关闭服务器...");
    
    // 设置停止标志，主循环会退出
    running = false;
    
    // 关闭控制监听套接字，让 epoll_wait 尽快返回
    if(controlServerSocket >= 0) {
        shutdown(controlServerSocket, SHUT_RDWR);  // 关闭监听套接字
    }
    
    // 关闭数据监听套接字
    if(dataServerSocket >= 0) {
        shutdown(dataServerSocket, SHUT_RDWR);
    }
    
    // 给一点时间让响应发送出去和 epoll 退出
    usleep(100000);  // 100ms
    
    // 调用 stop() 清理所有资源
    stop();
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
    // PASV -> 数据传输 -> (PASV 自动失效) -> 下次传输需要新的 PASV
    int oldPasvListenSock = -1;
    int oldPasvPort = -1;
    {
        std::lock_guard<mutex> lg(sessionMutex);
        auto it = sessions.find(clientSock);
        if(it != sessions.end() && it->second) {
            if(it->second->pasvListenSock >= 0) {
                oldPasvListenSock = it->second->pasvListenSock;
            }
            if(it->second->pasvPort > 0) {
                oldPasvPort = it->second->pasvPort;
            }
        }
    }
    
    // 关闭旧的监听套接字（如果有）
    if(oldPasvListenSock >= 0) {
        close(oldPasvListenSock);
        log("关闭旧的 PASV 监听套接字，端口：" + std::to_string(oldPasvPort));
    }
    
    int pasvPort = allocatePasvPort();
    
    // 创建 PASV 监听套接字，绑定到分配的端口
    int pasvListenSock = socket(AF_INET, SOCK_STREAM, 0);
    if(pasvListenSock < 0) {
        string errResp = "425 无法创建数据监听套接字\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    int optval = 1;
    setsockopt(pasvListenSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    
    struct sockaddr_in pasvAddr;
    memset(&pasvAddr, 0, sizeof(pasvAddr));
    pasvAddr.sin_family = AF_INET;
    pasvAddr.sin_port = htons(pasvPort);
    pasvAddr.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(pasvListenSock, (struct sockaddr*)&pasvAddr, sizeof(pasvAddr)) < 0) {
        close(pasvListenSock);
        string errResp = "425 无法绑定数据端口\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    if(listen(pasvListenSock, 1) < 0) {
        close(pasvListenSock);
        string errResp = "425 无法监听数据端口\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    // 获取客户端会话信息并保存监听套接字
    string clientIp = "";
    {
        std::lock_guard<mutex> lg(sessionMutex);
        auto it = sessions.find(clientSock);
        if(it != sessions.end() && it->second) {
            it->second->pasvPort = pasvPort;
            it->second->pasvMode = true;
            it->second->pasvListenSock = pasvListenSock;  // 保存监听套接字
            clientIp = it->second->clientIp;  // 记录客户端 IP
        }
    }
    
    // 如果客户端是通过 127.0.0.1 连接的，返回 127.0.0.1，否则返回局域网 IP
    string ip = "127.0.0.1";  // 默认使用本地回环地址
    if(clientIp != "127.0.0.1") {
        // 非本地客户端，使用服务器实际 IP
        ip = serverIp;
    }
    
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
    FtpSession* session = nullptr;
    int pasvListenSock = -1;
    
    // 获取会话和 PASV 监听套接字
    {
        std::lock_guard<mutex> lg(sessionMutex);
        auto it = sessions.find(clientSock);
        if(it != sessions.end() && it->second) {
            session = it->second;
            pasvListenSock = session->pasvListenSock;
        }
    }
    
    if(!session) {
        string errResp = "500 会话不存在\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    // 检查是否已进入 PASV 模式且有有效的监听套接字
    if(!session->pasvMode || session->pasvPort <= 0 || pasvListenSock < 0) {
        string errResp = "425 请先使用 PASV 命令进入被动模式\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }

    string resp = "150 准备发送目录列表\r\n";
    send(clientSock, resp.c_str(), resp.length(), 0);
    
    // 设置监听套接字超时（5 秒）
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(pasvListenSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    // 接受客户端数据连接
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int dataSock = accept(pasvListenSock, (struct sockaddr*)&clientAddr, &clientLen);
    
    // 关闭并清理 PASV 监听套接字（在会话中清除）
    close(pasvListenSock);
    {
        std::lock_guard<mutex> lg(sessionMutex);
        session->pasvListenSock = -1;
        session->pasvPort = -1;
        session->pasvMode = false;
    }
    
    if(dataSock < 0) {
        string errResp = "425 无法打开数据连接\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    // 获取目录列表
    string dirList = listDirectory(session->workingDir);
    
    // 通过数据连接发送数据
    sendDataToClient(dataSock, dirList);
    
    // 关闭数据连接
    close(dataSock);
    session->dataSock = -1;
    
    // 发送 226 完成响应
    string completeResp = "226 传输完成\r\n";
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
        string resp = "500 需要文件名参数\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
        return;
    }
    
    FtpSession* session = nullptr;
    int pasvListenSock = -1;
    
    // 获取会话和 PASV 监听套接字
    {
        std::lock_guard<mutex> lg(sessionMutex);
        auto it = sessions.find(clientSock);
        if(it != sessions.end() && it->second) {
            session = it->second;
            pasvListenSock = session->pasvListenSock;
        }
    }
    
    if(!session) {
        string errResp = "500 会话不存在\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    // 检查是否已进入 PASV 模式且有有效的监听套接字
    if(!session->pasvMode || session->pasvPort <= 0 || pasvListenSock < 0) {
        string errResp = "425 请先使用 PASV 命令进入被动模式\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    // 构建文件路径
    string filePath = session->workingDir + "/" + filename;
    if(filePath[0] == '/' && filePath[1] == '/') {
        filePath = filePath.substr(1);
    }

    // 打开文件获取文件描述符，O_RDONLY 表示只读方式打开
    int fileFd = open(filePath.c_str(), O_RDONLY);
    // 检查文件描述符是否有效，小于 0 表示打开失败
    if(fileFd < 0) {
        string errResp = "550 文件不存在或无法读取：" + filename + "\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        // 清理 PASV 状态
        close(pasvListenSock);
        {
            std::lock_guard<mutex> lg(sessionMutex);
            session->pasvListenSock = -1;
            session->pasvPort = -1;
            session->pasvMode = false;
        }
        return;
    }
    
    // 获取文件大小，用于后续 sendfile 传输和日志显示
    struct stat fileStat;
    // fstat 通过文件描述符获取文件状态信息
    if(fstat(fileFd, &fileStat) < 0) {
        close(fileFd);  // 关闭文件描述符释放资源
        string errResp = "550 无法获取文件大小：" + filename + "\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        // 清理 PASV 状态
        close(pasvListenSock);
        {
            std::lock_guard<mutex> lg(sessionMutex);
            session->pasvListenSock = -1;
            session->pasvPort = -1;
            session->pasvMode = false;
        }
        return;
    }
    // 从 fileStat 结构体中获取文件大小（字节数）
    off_t fileSize = fileStat.st_size;
    
    string resp = "150 准备发送文件 " + filename + " (" + std::to_string(fileSize) + " 字节)\r\n";
    send(clientSock, resp.c_str(), resp.length(), 0);
    
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(pasvListenSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    // 接受客户端数据连接
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int dataSock = accept(pasvListenSock, (struct sockaddr*)&clientAddr, &clientLen);
    
    // 关闭并清理 PASV 监听套接字
    close(pasvListenSock);
    {
        std::lock_guard<mutex> lg(sessionMutex);
        session->pasvListenSock = -1;
        session->pasvPort = -1;
        session->pasvMode = false;
    }
    
    if(dataSock < 0) {
        close(fileFd);  // 关闭文件描述符释放资源
        string errResp = "425 无法打开数据连接\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    off_t offset = 0;  // 设置文件读取偏移量为 0，从文件开头开始传输
    ssize_t sentBytes = sendfile(dataSock, fileFd, &offset, fileSize);
    
    // 检查 sendfile 返回值，小于 0 表示传输失败
    if(sentBytes < 0) {
        log("sendfile 传输失败");  // 记录错误日志
    } else {
        // 记录成功传输的字节数到日志

        string logMsg = "使用 sendfile 发送 " + std::to_string(sentBytes) + " 字节";
        log(logMsg);
    }
    
    // 关闭文件描述符释放资源
    close(fileFd);
    
    // 关闭数据连接
    close(dataSock);
    session->dataSock = -1;
    
    // 发送 226 完成响应
    string completeResp = "226 传输完成\r\n";
    send(clientSock, completeResp.c_str(), completeResp.length(), 0);
}

void Server::handleStorCommand(int clientSock, const string& filename) {
    if(filename.empty()) {
        string resp = "500 需要文件名参数\r\n";
        send(clientSock, resp.c_str(), resp.length(), 0);
        return;
    }
    
    FtpSession* session = nullptr;
    int pasvListenSock = -1;
    
    // 获取会话和 PASV 监听套接字
    {
        std::lock_guard<mutex> lg(sessionMutex);
        auto it = sessions.find(clientSock);
        if(it != sessions.end() && it->second) {
            session = it->second;
            pasvListenSock = session->pasvListenSock;
        }
    }
    
    if(!session) {
        string errResp = "500 会话不存在\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    if(!session->pasvMode || session->pasvPort <= 0 || pasvListenSock < 0) {
        string errResp = "425 请先使用 PASV 命令进入被动模式\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    string resp = "150 准备接收文件 " + filename + "\r\n";
    send(clientSock, resp.c_str(), resp.length(), 0);
    
    // 设置监听套接字超时（5 秒）
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(pasvListenSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    // 接受客户端数据连接
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int dataSock = accept(pasvListenSock, (struct sockaddr*)&clientAddr, &clientLen);
    
    // 关闭并清理 PASV 监听套接字
    close(pasvListenSock);
    {
        std::lock_guard<mutex> lg(sessionMutex);
        session->pasvListenSock = -1;
        session->pasvPort = -1;
        session->pasvMode = false;
    }
    
    if(dataSock < 0) {
        string errResp = "425 无法打开数据连接\r\n";
        send(clientSock, errResp.c_str(), errResp.length(), 0);
        return;
    }
    
    // 通过数据连接接收文件内容
    string content = receiveDataFromClient(dataSock, 1024 * 1024);
    
    // 关闭数据连接
    close(dataSock);
    session->dataSock = -1;
    
    // 构建文件路径
    string filePath = session->workingDir + "/" + filename;
    if(filePath[0] == '/' && filePath[1] == '/') {
        filePath = filePath.substr(1);
    }

    // 保存文件
    if(saveFile(filePath, content)) {
        string completeResp = "226 传输完成，文件已保存\r\n";
        send(clientSock, completeResp.c_str(), completeResp.length(), 0);
    } else {
        string errResp = "550 无法保存文件\r\n";
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
            // 关闭数据连接
            if(it->second->dataSock >= 0) {
                close(it->second->dataSock);
            }
            // 关闭 PASV 监听套接字（如果存在）
            if(it->second->pasvListenSock >= 0) {
                close(it->second->pasvListenSock);
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
    cout << "║  PWD - 显示当前目录                     ║\n";
    cout << "║  CWD - 切换目录                         ║\n";
    cout << "║  CDUP - 返回上级目录                    ║\n";
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
    
    log("正在关闭所有客户端会话...");
    
    // 关闭所有会话
    {
        std::lock_guard<mutex> lg(sessionMutex);
        for(auto& [sock, session] : sessions) {
            if(session) {
                // 发送关闭通知给客户端
                string msg = "421 服务器正在关闭...\r\n";
                send(sock, msg.c_str(), msg.length(), 0);
                // 关闭数据连接
                if(session->dataSock >= 0) {
                    close(session->dataSock);
                    session->dataSock = -1;
                }
                // 关闭 PASV 监听套接字
                if(session->pasvListenSock >= 0) {
                    close(session->pasvListenSock);
                    session->pasvListenSock = -1;
                }
                delete session;
            }
            // 关闭控制连接
            close(sock);
        }
        sessions.clear();
    }

    // 关闭 epoll
    if(epollFd >= 0) {
        close(epollFd);
        epollFd = -1;
    }
    
    // 关闭控制监听套接字
    if(controlServerSocket >= 0) {
        close(controlServerSocket);
        controlServerSocket = -1;
    }
    
    // 关闭数据监听套接字
    if(dataServerSocket >= 0) {
        close(dataServerSocket);
        dataServerSocket = -1;
    }
    
    log("FTP 服务器已完全关闭");
}

void Server::releasePasvPort(int port) {
   
    string msg = "释放被动模式端口：" + std::to_string(port) + "\n";
    cout << msg;
}