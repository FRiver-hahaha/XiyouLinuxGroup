#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <thread>
#include <atomic>
#include <random>
#include <stdexcept>
#include <cstring>
#include <ifaddrs.h>
#include <fcntl.h>

class ConnectionManager {
public:

    /*
        创建服务器监听 Socket:
        绑定指定端口，设置 SO_REUSEADDR 选项允许端口复用。
        返回监听 Socket 的文件描述符。
        失败时抛出 std::runtime_error 异常。
    */

    static int createServerSocket(int port) {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            throw std::runtime_error("Failed to create socket");
        }
        
        int opt = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            close(serverSocket);
            throw std::runtime_error("setsockopt failed");
        }
        
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);
        
        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            close(serverSocket);
            throw std::runtime_error("Bind failed");
        }
        
        if (listen(serverSocket, 5) < 0) {
            close(serverSocket);
            throw std::runtime_error("Listen failed");
        }
        
        return serverSocket;
    }
    
    static int createPassiveSocket(int& port) {
        int passiveSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (passiveSocket < 0) {
            throw std::runtime_error("Failed to create passive socket");
        }
        
        int opt = 1;
        setsockopt(passiveSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = 0;
        
        if (bind(passiveSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(passiveSocket);
            throw std::runtime_error("Bind failed for passive socket");
        }
        
        if (listen(passiveSocket, 1) < 0) {
            close(passiveSocket);
            throw std::runtime_error("Listen failed for passive socket");
        }
        
        socklen_t len = sizeof(addr);
        if (getsockname(passiveSocket, (struct sockaddr*)&addr, &len) < 0) {
            close(passiveSocket);
            throw std::runtime_error("getsockname failed");
        }
        port = ntohs(addr.sin_port);
        
        return passiveSocket;
    }
    
    static int acceptConnection(int serverSocket, int timeoutSeconds = 30) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        memset(&clientAddr, 0, clientLen);
        
        // Set timeout for accept
        struct timeval tv;
        tv.tv_sec = timeoutSeconds;
        tv.tv_usec = 0;
        setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
        
        return accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    }
    
    /*
        通过链表遍历电脑内的所有网络接口，
        遇到非回环地址则返回。
    */

    static std::string getLocalIP() {
        struct ifaddrs *ifaddr, *ifa;
        
        if (getifaddrs(&ifaddr) == -1) {
            return "127.0.0.1";
        }
        
        std::string result = "127.0.0.1";
        
        for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == nullptr) continue;
            
            if (ifa->ifa_addr->sa_family == AF_INET) {
                struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(addr->sin_addr), ip, sizeof(ip));// ipv4
                
                std::string ipStr(ip);
                if (ipStr != "127.0.0.1" && ipStr.find("127.") != 0) {
                    result = ipStr;
                    break;
                }
                if (ipStr == "127.0.0.1") {
                    result = ipStr;
                }
            }
        }
        
        freeifaddrs(ifaddr);
        return result;
    }
    
    static std::string formatPassiveResponse(const std::string& ip, int port) {
        std::string formattedIP;
        for (char c : ip) {
            if (c == '.') formattedIP += ',';
            else formattedIP += c;
        }
        
        int p1 = port / 256;
        int p2 = port % 256;
        
        return formattedIP + "," + std::to_string(p1) + "," + std::to_string(p2);
    }
};

#endif