#include "Server.h"
#include <arpa/inet.h>

Server::Server(int port) 
    : serverSocket(-1), port(port) {
    signal(SIGPIPE, SIG_IGN);

    threadPool = std::make_unique<ThreadPool> (4);

} 

bool Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        if(logCallback) logCallback("无法创建套接字");
        else log("无法创建套接字");
        return false;
    }

    int optval = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);// 确定端口
    serverAddr.sin_addr.s_addr = INADDR_ANY;// 随机ip地址

    if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(serverSocket);
        if(logCallback) logCallback("无法绑定套接字和ip地址");
        else log("无法绑定套接字和ip地址");
        return false;
    }

    if(listen(serverSocket, SOMAXCONN) < 0) {
        close(serverSocket);
        if(logCallback) logCallback("无法监听");
        else log("无法监听");
        return false;
    }

    printServerInfo();

    if(logCallback) {
        logCallback("服务器启动成功，监听端口: " + std::to_string(port));
        logCallback("线程池已初始化，工作线程数: 4");
    }

    return true;
}

void Server::run() {
    // ServerThread = std::thread(&Server::console, this);

    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    if (logCallback) logCallback("等待客户端链接...");
    else log("等待客户端链接...");

    while(running) {
        memset(&clientAddr, 0, clientLen);

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

        if(!running) break;

        if(clientSocket < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;  // 超时，继续循环
            }
            if(!running) break;
            if (logCallback) logCallback("接受连接失败");
            else log("接受连接失败");
            continue;
        }
        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);
        
        string logMsg = "新客户端连接: " + string(clientIp) + ":" + std::to_string(clientPort);
        if (logCallback) logCallback(logMsg);
        else log(logMsg);

        {
            std::lock_guard<mutex> lg(clientMutex);
            clientSockets.push_back(clientSocket);

            if(clientCountCallback) clientCountCallback(clientSockets.size());
        }

        threadPool->submit(
            [this, clientSocket]() {
            talkWithClient(clientSocket);
        }, [this, clientSocket, clientIp, clientPort]() {
            string logMsg = "客户端断开: " + std::string(clientIp) + ":" + std::to_string(clientPort);
            if (logCallback) logCallback(logMsg);
            else log(logMsg);
            removeClient(clientSocket);
            close(clientSocket);
        });

    }

    if (logCallback) logCallback("服务器已停止接受新连接");
    else log("客户端已关闭链接...");

    // if(ServerThread.joinable()) {
    //     ServerThread.join();
    // }

    if (logCallback) logCallback("已关闭服务器");
    else log("已关闭服务器");
}

void Server::stop() {
    if(!running) return;

    running = false;
    
    {
        std::lock_guard<mutex> lg(clientMutex);
        for(auto sock : clientSockets) {
            string meg = "服务器正在关闭...\n";
            send(sock, meg.c_str(), meg.length(), 0);
            close(sock);
        } 
        clientSockets.clear();
    }

    close(serverSocket);
    if (logCallback) logCallback("服务器已关闭");
    else log("服务器已关闭");
}

void Server::talkWithClient(const int clientSock) {
    char buffer[SIZE_BUFFER];
    string sentence;
    string command;

    sentence = "欢迎进入名字-年龄服务器\n输入HELP获取可用指令...";
    send(clientSock, sentence.c_str(), sentence.length(), 0);

    while(running) {
        memset(buffer, 0, sizeof(buffer));
        size_t bytesReceived = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        
        if(bytesReceived <= 0) {
            if(bytesReceived == 0) {
                if (logCallback) logCallback("客户端主动关闭");
                else log("客户端主动关闭");
            }else {
                if (logCallback) logCallback("recv错误");
                else log("recv错误");
            }
            break;
        }

        buffer[bytesReceived] = '\0';

        command = buffer;
        command.erase(command.find_last_not_of("\r\n") + 1);

        if(command.empty()) {
            continue;
        }

        string logMsg = "接收到: " + command + " 指令";
        if (logCallback) logCallback(logMsg);
        else log(logMsg);

        sentence = parseCommand(command);// 解析好的命令，作为参数，发送给客户端
        send(clientSock, sentence.c_str(), sentence.length(), 0);

        if (commandCallback) {
            commandCallback(command, sentence);
        }

        if(sentence == "Oops! BYE~") {
            break;
        }
    }
}

string Server::parseCommand(const string& command) {
    return cmdHandle.execute(command);
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
    cout << "║        服务器已启动，等待连接...       ║\n";
    cout << "╠════════════════════════════════════════╣\n";
    cout << "║ 端口: " << std::setw(33) << std::left << port << "║\n";
    
    // 打印本机回环地址
    cout << "║ 本地访问: 127.0.0.1:" << port;
    cout << std::string(20 - std::to_string(port).length(), ' ') << "║\n";
    
    // 打印所有局域网IP
    auto ips = getLocalIps();
    for (const auto& ip : ips) {
        cout << "║ 局域网访问: " << ip << ":" << port;
        int padding = 19 - ip.length() - std::to_string(port).length();
        if (padding > 0) cout << std::string(padding, ' ');
        cout << "║\n";
    }
    
    cout << "╠════════════════════════════════════════╣\n";
    cout << "║ 其他主机连接命令:                      ║\n";
    
    // 提供客户端连接命令示例
    if (!ips.empty()) {
        cout << "║ ./nameclient " << ips[0] << " " << port;
        int padding = 21 - ips[0].length() - std::to_string(port).length();
        if (padding > 0) cout << std::string(padding, ' ');
        cout << "║\n";
    }
    
    cout << "╚════════════════════════════════════════╝\n\n";
}

void Server::removeClient(const int clientSocket) {
    std::lock_guard<mutex> lg(clientMutex);
    clientSockets.erase(
        std::remove(clientSockets.begin(), clientSockets.end(), clientSocket),
        clientSockets.end()
    );
}

int Server::getClientCount() {
    std::lock_guard<mutex> lg(clientMutex);
    return clientSockets.size();
}
