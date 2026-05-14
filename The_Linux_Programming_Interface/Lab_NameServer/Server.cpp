#include "Server.h"
#include <arpa/inet.h>

Server::Server(int port) 
    : serverSocket(-1), port(port) {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, [](int) {
        log("收到 SIGINT 信号");
    });

    threadPool = std::make_unique<ThreadPool> (4);

} 

bool Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        log("无法创建套接字");
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
        log("无法绑定套接字和ip地址");
        return false;
    }

    if(listen(serverSocket, SOMAXCONN) < 0) {
        close(serverSocket);
        log("无法监听");
        return false;
    }

    printServerInfo();

    return true;
}

void Server::run() {
    ServerThread = std::thread(&Server::console, this);

    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    log("等待客户端链接...");

    while(running) {
        memset(&clientAddr, 0, clientLen);

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        int  clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

        if(!running) break;

        if(clientSocket < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;  // 超时，继续循环
            }
            if(!running) break;
            log("接受连接失败");
            continue;
        }
        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);
        log("新客户端连接: " + string(clientIp) + ":" + std::to_string(clientPort));

        {
            std::lock_guard<mutex> lg(clientMutex);
            clientSockets.push_back(clientSocket);
        }

        threadPool->submit(
            [this, clientSocket]() {
            talkWithClient(clientSocket);
        }, [this, clientSocket, clientIp, clientPort]() {
            log("客户端断开: " + std::string(clientIp) + ":" + std::to_string(clientPort));
            removeClient(clientSocket);
            close(clientSocket);
        });

    }

    log("客户端已关闭链接...");

    if(ServerThread.joinable()) {
        ServerThread.join();
    }

    log("已关闭服务器");
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
    log("服务器已关闭");
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
                log("客户端主动关闭");
            }else {
                log("recv错误");
            }
            break;
        }

        buffer[bytesReceived] = '\0';

        command = buffer;
        command.erase(command.find_last_not_of("\r\n") + 1);

        if(command.empty()) {
            continue;
        }

        log("接收到: " + command + " 指令");
        sentence = parseCommand(command);
        send(clientSock, sentence.c_str(), sentence.length(), 0);
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

void Server::console() {
    string command;
    
    cout << "\n========================================" << endl;
    cout << "  服务器管理控制台已启动" << endl;
    cout << "  输入 'help' 查看可用命令" << endl;
    cout << "========================================" << endl;
    
    while(running) {
        cout << "\nadmin> ";
        std::getline(cin, command);
        
        if(command.empty()) continue;
        
        if(command == "help") {
            cout << "\n可用管理命令:" << endl;
            cout << "  help        - 显示此帮助信息" << endl;
            cout << "  status      - 显示服务器状态" << endl;
            cout << "  clients     - 显示连接的客户端列表" << endl;
            cout << "  info        - 显示服务器网络信息" << endl;
            cout << "  quit        - 关闭服务器" << endl;
            
        } else if(command == "status") {
            cout << "\n服务器状态:" << endl;
            cout << "  运行状态: " << (running ? "运行中" : "已停止") << endl;
            cout << "  监听端口: " << port << endl;
            
            size_t clientCount;
            {
                std::lock_guard<mutex> lock(clientMutex);
                clientCount = clientSockets.size();
            }
            cout << "  连接客户端数: " << clientCount << endl;
            
        } else if(command == "clients") {
            printClientList();
            
        } else if(command == "info") {
            printServerInfo();
            
        } else if(command == "quit" || command == "exit") {
            cout << "\n正在关闭服务器..." << endl;
            stop();
            break;
            
        } else {
            cout << "未知命令: " << command << " (输入 'help' 查看可用命令)" << endl;
        }
    }
    
    cout << "管理控制台已关闭" << endl;
}

void Server::removeClient(const int clientSocket) {
    std::lock_guard<mutex> lg(clientMutex);
    clientSockets.erase(
        std::remove(clientSockets.begin(), clientSockets.end(), clientSocket),
        clientSockets.end()
    );
}

void Server::printClientList() {
    std::lock_guard<std::mutex> lock(clientMutex);
    
    cout << "\n当前连接的客户端 (" << clientSockets.size() << "):" << endl;
    cout << "----------------------------------------" << endl;
    
    if(clientSockets.empty()) {
        cout << "  没有客户端连接" << endl;
    } else {
        for(size_t i = 0; i < clientSockets.size(); ++i) {
            cout << "  [" << i + 1 << "] Socket FD: " << clientSockets[i] << endl;
        }
    }
    cout << "----------------------------------------" << endl;
}

std::atomic<bool> g_running{true};

void signalHandler(int signum) {
    if (signum == SIGINT) {
        log("收到 SIGINT 信号");
        g_running = false;
    }
}

int main(int argc, char* argv[]) {
    int port = 8888;
    
    if(argc > 1) {
        port = std::stoi(argv[1]);
    }

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    Server server(port);

    if(!server.start()) {
        std::runtime_error("无法启动服务器");
        return 1;
    }

    server.run();

    return 0;
}