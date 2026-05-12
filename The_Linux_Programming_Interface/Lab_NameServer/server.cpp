#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>

class KeyValueStore {
private:
    std::map<std::string, std::string> store;

public:
    // 添加或修改键值对
    std::string set(const std::string& key, const std::string& value) {
        store[key] = value;
        return "OK: SET " + key + " = " + value;
    }

    // 获取值
    std::string get(const std::string& key) {
        auto it = store.find(key);
        if (it != store.end()) {
            return "VALUE: " + it->second;
        }
        return "ERROR: Key not found";
    }

    // 删除键值对
    std::string del(const std::string& key) {
        auto it = store.find(key);
        if (it != store.end()) {
            store.erase(it);
            return "OK: DELETED " + key;
        }
        return "ERROR: Key not found";
    }

    // 列出所有键值对
    std::string list() {
        if (store.empty()) {
            return "EMPTY: No key-value pairs stored";
        }
        
        std::stringstream ss;
        ss << "LIST:\n";
        for (const auto& pair : store) {
            ss << "  " << pair.first << " = " << pair.second << "\n";
        }
        return ss.str();
    }
};

class KeyValueServer {
private:
    int server_socket;
    int port;
    KeyValueStore kv_store;

    // 处理客户端请求
    std::string process_command(const std::string& command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "SET") {
            std::string key, value;
            iss >> key;
            std::getline(iss, value);
            // 去除前导空格
            if (!value.empty() && value[0] == ' ') {
                value = value.substr(1);
            }
            if (key.empty() || value.empty()) {
                return "ERROR: Usage: SET <key> <value>";
            }
            return kv_store.set(key, value);
        }
        else if (cmd == "GET") {
            std::string key;
            iss >> key;
            if (key.empty()) {
                return "ERROR: Usage: GET <key>";
            }
            return kv_store.get(key);
        }
        else if (cmd == "DEL" || cmd == "DELETE") {
            std::string key;
            iss >> key;
            if (key.empty()) {
                return "ERROR: Usage: DEL <key>";
            }
            return kv_store.del(key);
        }
        else if (cmd == "LIST") {
            return kv_store.list();
        }
        else if (cmd == "QUIT" || cmd == "EXIT") {
            return "BYE";
        }
        else if (cmd == "HELP") {
            return "Commands: SET <key> <value>, GET <key>, DEL <key>, LIST, QUIT, HELP";
        }
        else {
            return "ERROR: Unknown command. Type HELP for available commands.";
        }
    }

public:
    KeyValueServer(int port = 8888) : port(port), server_socket(-1) {}

    bool start() {
        // 创建 socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }

        // 设置 socket 选项，允许地址重用
        int opt = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "Failed to set socket options" << std::endl;
            return false;
        }

        // 绑定地址和端口
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Failed to bind to port " << port << std::endl;
            return false;
        }

        // 开始监听
        if (listen(server_socket, 5) < 0) {
            std::cerr << "Failed to listen" << std::endl;
            return false;
        }

        std::cout << "Key-Value Server started on port " << port << std::endl;
        std::cout << "Waiting for connections..." << std::endl;

        return true;
    }

    void run() {
        while (true) {
            // 接受客户端连接
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            
            int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
            if (client_socket < 0) {
                std::cerr << "Failed to accept connection" << std::endl;
                continue;
            }

            std::cout << "Client connected from " << inet_ntoa(client_addr.sin_addr) 
                      << ":" << ntohs(client_addr.sin_port) << std::endl;

            // 处理客户端请求
            handle_client(client_socket);

            close(client_socket);
            std::cout << "Client disconnected" << std::endl;
        }
    }

    void handle_client(int client_socket) {
        char buffer[1024];
        std::string response;
        
        // 发送欢迎消息
        response = "Welcome to Key-Value Store Server\nType HELP for available commands.\n";
        send(client_socket, response.c_str(), response.length(), 0);

        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            
            if (bytes_received <= 0) {
                break;  // 客户端断开连接
            }

            std::string command(buffer);
            // 去除换行符
            command.erase(command.find_last_not_of("\r\n") + 1);

            if (command.empty()) {
                continue;
            }

            std::cout << "Received: " << command << std::endl;
            
            response = process_command(command) + "\n";
            send(client_socket, response.c_str(), response.length(), 0);

            if (response == "BYE\n") {
                break;
            }
        }
    }

    ~KeyValueServer() {
        if (server_socket != -1) {
            close(server_socket);
        }
    }
};

int main(int argc, char* argv[]) {
    int port = 8888;
    
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }

    KeyValueServer server(port);
    
    if (!server.start()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    server.run();

    return 0;
}