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

/*

    服务端入口：
    设置控制链接端口可数据链接端口
    创建服务器对象
    启动服务器，做好错误排查

*/

int main(int argc, char* argv[]) {

    int controlPort = 2100;   // 控制连接端口
    int dataPort = 2000;      // 数据连接端口（主动模式）
    
    // 确定两个端口的数值
    if(argc > 1) {
        controlPort = std::stoi(argv[1]);
    }
    if(argc > 2) {
        dataPort = std::stoi(argv[2]);
    }

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    Server server(controlPort, dataPort);

    if(!server.start()) {
        std::runtime_error("无法启动 FTP 服务器");
        return 1;
    }

    server.run();

    return 0;
}