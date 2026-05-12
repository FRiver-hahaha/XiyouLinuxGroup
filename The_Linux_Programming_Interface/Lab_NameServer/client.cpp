#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class KeyValueClient {
private:
    int client_socket;
    std::string server_address;
    int server_port;

public:
    KeyValueClient(const std::string& address = "127.0.0.1", int port = 8888) 
        : server_address(address), server_port(port), client_socket(-1) {}

    bool connect_to_server() {
        // 创建 socket
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1) {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }

        // 设置服务器地址
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port);
        
        if (inet_pton(AF_INET, server_address.c_str(), &server_addr.sin_addr) <= 0) {
            std::cerr << "Invalid address" << std::endl;
            return false;
        }

        // 连接到服务器
        if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Connection failed" << std::endl;
            return false;
        }

        std::cout << "Connected to server at " << server_address << ":" << server_port << std::endl;
        
        // 接收欢迎消息
        receive_response();
        
        return true;
    }

    void receive_response() {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0) {
            std::cout << buffer;
        }
    }

    std::string send_command(const std::string& command) {
        if (client_socket == -1) {
            return "Not connected to server";
        }

        // 发送命令
        std::string cmd_with_newline = command + "\n";
        send(client_socket, cmd_with_newline.c_str(), cmd_with_newline.length(), 0);

        // 接收响应
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0) {
            std::string response(buffer);
            // 去除末尾换行符
            response.erase(response.find_last_not_of("\r\n") + 1);
            return response;
        }
        
        return "Connection closed by server";
    }

    void interactive_mode() {
        std::string command;
        std::cout << "Enter commands (type 'help' for available commands, 'quit' to exit):" << std::endl;
        
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, command);
            
            if (command.empty()) {
                continue;
            }
            
            std::string response = send_command(command);
            std::cout << response << std::endl;
            
            if (command == "QUIT" || command == "quit" || response == "BYE") {
                break;
            }
        }
    }

    void test_commands() {
        std::cout << "\n=== Testing Key-Value Store ===" << std::endl;
        
        // 测试 SET 命令
        std::cout << "\n1. Testing SET commands:" << std::endl;
        std::cout << send_command("SET name Alice") << std::endl;
        std::cout << send_command("SET age 25") << std::endl;
        std::cout << send_command("SET city Beijing") << std::endl;
        
        // 测试 GET 命令
        std::cout << "\n2. Testing GET commands:" << std::endl;
        std::cout << send_command("GET name") << std::endl;
        std::cout << send_command("GET age") << std::endl;
        std::cout << send_command("GET nonexistent") << std::endl;
        
        // 测试 LIST 命令
        std::cout << "\n3. Testing LIST command:" << std::endl;
        std::cout << send_command("LIST") << std::endl;
        
        // 测试修改
        std::cout << "\n4. Testing UPDATE command:" << std::endl;
        std::cout << send_command("SET age 26") << std::endl;
        std::cout << send_command("GET age") << std::endl;
        
        // 测试删除
        std::cout << "\n5. Testing DEL command:" << std::endl;
        std::cout << send_command("DEL city") << std::endl;
        std::cout << send_command("LIST") << std::endl;
        
        // 测试删除不存在的键
        std::cout << "\n6. Testing DEL on non-existent key:" << std::endl;
        std::cout << send_command("DEL nonexistent") << std::endl;
        
        std::cout << "\n=== Test Complete ===" << std::endl;
    }

    void disconnect() {
        if (client_socket != -1) {
            send_command("QUIT");
            close(client_socket);
            client_socket = -1;
            std::cout << "Disconnected from server" << std::endl;
        }
    }

    ~KeyValueClient() {
        disconnect();
    }
};

int main(int argc, char* argv[]) {
    std::string address = "127.0.0.1";
    int port = 8888;
    
    if (argc > 1) {
        address = argv[1];
    }
    if (argc > 2) {
        port = std::stoi(argv[2]);
    }

    KeyValueClient client(address, port);
    
    if (!client.connect_to_server()) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }

    // 运行自动测试
    client.test_commands();
    
    // 进入交互模式
    std::cout << "\n--- Interactive Mode ---" << std::endl;
    client.interactive_mode();

    return 0;
}