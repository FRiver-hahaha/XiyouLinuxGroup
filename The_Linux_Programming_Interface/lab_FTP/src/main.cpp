// 2026.5.10 完成项目构建

// 明日完成 connection_manger.h 的重写

#include <iostream>
#include "ftp_server.h"

int main() {
    try {
        FTPServer server(2100);
        std::cout << "Starting FTP Server..." << std::endl;
        std::cout << "Connect with: ftp 127.0.0.1 2100" << std::endl;
        std::cout << "username=admin, password=admin123" << std::endl;
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}