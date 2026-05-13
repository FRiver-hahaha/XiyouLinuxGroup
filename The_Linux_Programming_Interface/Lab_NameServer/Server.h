#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <atomic>
#include <iostream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <functional>
#include "Store.h"
#include "CommandHandler.h"

using std::cout;    using std::endl;
using std::string;  using hash_map = std::unordered_map<string, string>;

constexpr int SIZE_BUFFER = 1024;

class Server {

    int serverSocket;
    int clientSocket;
    int port;
    std::atomic<bool> running = true;
    Store store;
    CommandHandler cmdHandle{store};

public:
    Server(int port = 8888);

    bool start();// 用来监听客户端
    void run();// 用来建立和客户端的链接
    void stop();// 用来关闭连接
    void talkWithClient(const int clientSocket);// 与客户端进行i/o
    string parseCommand(const string& command);

    ~Server() {
        if(serverSocket != -1) {
            close(serverSocket);
        }
    }
};