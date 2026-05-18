#ifndef CLIENT_H
#define CLIENT_H

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdexcept>
#include <string>
#include <iostream>

using std::string; 
using std::cout;    using std::cin;

constexpr int SIZE_BUFFER = 1024;

class Client {
    
    int serverSocket;// 由客户端获取
    int clientSocket;
    int serverPort;
    string serverAddress;

public:
    Client(const string& address = "127.0.0.1", int port = 8888)
     : clientSocket(-1), serverPort(port), serverAddress(address) {}

    bool connectServer();
    void receiveResponse();
    string sendCommand(const string& command);
    void microShell();
    void disconnect();

    ~Client () {
        disconnect();
    }
};

#endif
