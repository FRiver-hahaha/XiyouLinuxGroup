#include "Client.h"

/*

    客户端入口：
    设置控制链接端口可数据链接端口
    创建客户端对象
    做好错误排查

*/

int main(int argc, char* argv[]) {
    
    std::string address = "127.0.0.1";
    int port = 2100;  // FTP 控制端口
    
    if (argc > 1) {
        address = argv[1];
    }
    if (argc > 2) {
        port = std::stoi(argv[2]);
    }

    Client client(address, port);
    
    if (!client.connectFtpServer()) {
        std::runtime_error("无法连接到 FTP 服务器");
        return 1;
    }

    std::cout << "\n进入 FTP 交互模式\n";
    client.microShell();

    return 0;
}