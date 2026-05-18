#include "Client.h"

bool Client::connectServer() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        throw std::runtime_error("无法创建套接字");
        return false;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        throw std::runtime_error("无法获取ip地址");
        return false;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        throw std::runtime_error("无法连接到服务器");
        return false;
    }

    cout << "欢迎进入服务器 " << serverAddress << ":" << serverPort << '\n';

    // 接受消息
    receiveResponse();
    return true;
}

void Client::receiveResponse() {
    char buffer[SIZE_BUFFER];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    buffer[bytesReceived] = '\0';

    if(bytesReceived > 0) {
        cout << buffer;
    }
}

string Client::sendCommand(const string& command) {
    if (clientSocket == -1) {
        return "无法连接到服务器";
    }

    // 发送命令
    string cmd = command + "\n";
    send(clientSocket, cmd.c_str(), cmd.length(), 0);

    // 接收响应
    char buffer[SIZE_BUFFER];
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received > 0) {
        string response(buffer);
        response.erase(response.find_last_not_of("\r\n") + 1);
        return response;
    }
    
    return "链接已被服务器关闭";
}

void Client::microShell() {
    string command;

    while (true) {
        cout << "> ";
        std::getline(cin, command);
        
        if (command.empty()) {
            continue;
        }
        
        string response = sendCommand(command);
        cout << response << '\n';
        
        if (command == "QUIT" || response == "Oops! BYE~") {
            break;
        }
    }
}

void Client::disconnect() {
    if(clientSocket != -1) {
        sendCommand("QUIT");
        close(clientSocket);
        clientSocket = -1;
        cout << "已关闭链接" << '\n';
    }
}