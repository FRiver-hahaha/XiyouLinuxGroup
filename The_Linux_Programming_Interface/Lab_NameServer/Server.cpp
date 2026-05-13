#include "Server.h"

Server::Server(int port) 
    : serverSocket(-1), clientSocket(-1), port(port), running(true), cmdHandle(store) {
    signal(SIGPIPE, SIG_IGN);

} 

bool Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        throw std::runtime_error("无法创建套接字");
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);// 确定端口
    serverAddr.sin_addr.s_addr = INADDR_ANY;// 随机ip地址

    if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(serverSocket);
        throw std::runtime_error("无法绑定套接字和ip地址");
    }

    if(listen(serverSocket, 4) < 0) {
        close(serverSocket);
        throw std::runtime_error("无法监听");
    }

    cout << "服务器端口: " << port << '\n';

    return true;
}

void Server::run() {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    memset(&clientAddr, 0, clientLen);
     
    while(running) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if(clientSocket < 0) {
            std::runtime_error("无法建立链接...");
            continue;
        }
        talkWithClient(clientSocket);
        cout << "客户端已加入链接..." << '\n';
        close(clientSocket);
        cout << "客户端已关闭链接..." << '\n';
    }

    
}

void Server::stop() {
    running = false;
    close(serverSocket);
}

void Server::talkWithClient(const int ClientSocket) {
    char buffer[SIZE_BUFFER];
    string sentence;
    string command;

    sentence = "欢迎进入名字-年龄服务器\n输入HELP获取可用指令...";
    send(clientSocket, sentence.c_str(), sentence.length(), 0);
    while(running) {
        memset(buffer, 0, sizeof(buffer));
        size_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if(bytesReceived <= 0) {
            if(bytesReceived == 0) {
                cout << "客户端主动关闭" << endl;
            }else {
                std::runtime_error("recv错误");
            }
            break;
        }

        buffer[bytesReceived] = '\0';

        command = buffer;
        command.erase(command.find_last_not_of("\r\n") + 1);

        if(command.empty()) {
            continue;
        }

        cout << "接收到: " << command << " 指令" << '\n';
        sentence = parseCommand(command);
        send(clientSocket, sentence.c_str(), sentence.length(), 0);
        if(sentence == "Oops! BYE~") {
            break;
        }
    }
}

string Server::parseCommand(const string& command) {
    std::istringstream iss(command);
    string cmd;
    string res;
    iss >> cmd;
    res = cmdHandle.execute(command);
    return res;
}