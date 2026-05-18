#include "Client.h"

int main(int argc, char* argv[]) {
    std::string address = "127.0.0.1";
    int port = 8888;
    
    if (argc > 1) {
        address = argv[1];
    }
    if (argc > 2) {
        port = std::stoi(argv[2]);
    }

    Client client(address, port);
    
    if (!client.connectServer()) {
        std::runtime_error("无法连接到服务器");
        return 1;
    }

    
    // 进入交互模式
    std::cout << "\n进入交互模式" << '\n';
    client.microShell();

    return 0;
}