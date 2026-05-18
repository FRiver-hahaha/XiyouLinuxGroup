// main_server.cpp - 命令行服务器入口点
#include "Server.h"
#include <atomic>
#include <signal.h>

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