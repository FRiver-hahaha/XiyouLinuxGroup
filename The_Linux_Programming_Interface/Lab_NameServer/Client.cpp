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

    welcome();

    return true;
}

void Client::welcome() {

}