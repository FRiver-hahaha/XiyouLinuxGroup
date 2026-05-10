#ifndef FTP_SERVER_H
#define FTP_SERVER_H

#include <string>
#include <atomic>
#include <memory>
#include <filesystem>
#include "ThreadPool.h"
#include "file_handler.h"
#include "connection_manager.h"
#include "ftp_commands.h"

namespace fs = std::filesystem;

class FTPServer {
private:
    int controlPort;
    FileHandler fileHandler;
    ThreadPool threadPool;
    std::atomic<bool> running;
    
    struct UserInfo {
        std::string username;
        std::string password;
        bool authenticated;
        int64_t restartOffset = 0;
    };
    
    void handleClient(int clientSocket);
    
public:
    FTPServer(int port = 2100);
    void start();
    void stop();
};

#endif