#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdexcept>
#include <string>
#include <iostream>

using std::string; 
using std::cout;

class Client {
    
    int serverSocket;
    int clientSocket;
    int serverPort;
    string serverAddress;

public:
    bool connectServer();
    void welcome();
};