// src/ftp_server.cpp
#include "ftp_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <fstream>

FTPServer::FTPServer(int port) : controlPort(port), threadPool(8), running(false) {
    // Use absolute path
    std::string rootPath = fs::absolute("./ftp_root").string();
    fileHandler = FileHandler(rootPath);
}

void FTPServer::handleClient(int clientSocket) {
    UserInfo user = {"", "", false, 0};
    char buffer[4096];
    std::string recvBuffer;
    int passiveSocket = -1;
    int dataPort = 0;
    
    // Get client IP
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    std::string clientIP = "127.0.0.1";
    if (getpeername(clientSocket, (struct sockaddr*)&clientAddr, &addrLen) == 0) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, sizeof(ipStr));
        clientIP = ipStr;
    }
    
    try {
        std::string welcome = FTPProtocol::getResponse(FTPResponseCode::READY_FOR_NEW_USER, 
                                                       "Welcome to Simple FTP Server");
        send(clientSocket, welcome.c_str(), welcome.length(), 0);
        
        while (running) {
            memset(buffer, 0, sizeof(buffer));
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            
            if (bytesRead <= 0) {
                if (bytesRead == 0) {
                    log("Client disconnected");
                } else {
                    log("Error reading from client: " + std::string(strerror(errno)));
                }
                break;
            }
            
            recvBuffer.append(buffer, bytesRead);
            
            size_t lineEnd;
            while ((lineEnd = recvBuffer.find("\r\n")) != std::string::npos) {
                std::string command = recvBuffer.substr(0, lineEnd);
                recvBuffer.erase(0, lineEnd + 2);
                
                if (command.empty()) continue;
                
                log("CMD: " + command);
                
                size_t spacePos = command.find(' ');
                std::string cmd = spacePos != std::string::npos ? 
                                 command.substr(0, spacePos) : command;
                std::string arg = spacePos != std::string::npos ? 
                                 command.substr(spacePos + 1) : "";
                
                // Convert to uppercase
                for (auto& c : cmd) c = toupper(c);
                
                FTPCommand ftpCmd = FTPProtocol::parseCommand(cmd);
                
                switch (ftpCmd) {
                    case FTPCommand::USER: {
                        user.username = arg;
                        user.authenticated = false;
                        
                        // Immediately reject anonymous users
                        if (arg.empty() || arg == "anonymous" || arg == "ftp") {
                            std::string response = "530 Anonymous login not allowed\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                        } else {
                            std::string response = FTPProtocol::getResponse(
                                FTPResponseCode::PASSWORD_NEEDED, "Please specify the password");
                            send(clientSocket, response.c_str(), response.length(), 0);
                        }
                        break;
                    }
                    
                    case FTPCommand::PASS: {
                        user.password = arg;
                        // Simple authentication
                        if ((user.username == "admin" && user.password == "admin123") ||
                            (user.username == "anonymous")) {
                            user.authenticated = true;
                            std::string response = FTPProtocol::getResponse(
                                FTPResponseCode::USER_LOGGED_IN, "Login successful");
                            send(clientSocket, response.c_str(), response.length(), 0);
                        } else {
                            std::string response = FTPProtocol::getResponse(
                                FTPResponseCode::NOT_IMPLEMENTED, "Login incorrect");
                            send(clientSocket, response.c_str(), response.length(), 0);
                        }
                        break;
                    }
                    
                    case FTPCommand::SYST: {
                        std::string response = FTPProtocol::getResponse(
                            FTPResponseCode::COMMAND_OK, "UNIX Type: L8");
                        send(clientSocket, response.c_str(), response.length(), 0);
                        break;
                    }
                    
                    case FTPCommand::FEAT: {
                        std::string features = "211-Features:\r\n"
                                             " PWD\r\n"
                                             " PASV\r\n"
                                             " LIST\r\n"
                                             " RETR\r\n"
                                             " STOR\r\n"
                                             " CWD\r\n"
                                             " SYST\r\n"
                                             " FEAT\r\n"
                                             " TYPE\r\n"
                                             "211 End\r\n";
                        send(clientSocket, features.c_str(), features.length(), 0);
                        break;
                    }
                    
                    case FTPCommand::TYPE: {
                        // Accept all TYPE commands
                        std::string response = FTPProtocol::getResponse(
                            FTPResponseCode::COMMAND_OK, "Switching to Binary mode");
                        send(clientSocket, response.c_str(), response.length(), 0);
                        break;
                    }
                    
                    case FTPCommand::PWD: {
                        if (!user.authenticated) {
                            std::string response = "530 Please login with USER and PASS\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                            break;
                        }
                        
                        std::string currentDir = fileHandler.currentDirectory();
                        std::string response = FTPProtocol::getResponse(
                            FTPResponseCode::COMMAND_OK, "\"" + currentDir + "\" is the current directory");
                        send(clientSocket, response.c_str(), response.length(), 0);
                        break;
                    }
                    
                    case FTPCommand::CWD: {
                        if (!user.authenticated) {
                            std::string response = "530 Please login with USER and PASS\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                            break;
                        }
                        
                        if (fileHandler.changeDirectory(arg)) {
                            std::string response = FTPProtocol::getResponse(
                                FTPResponseCode::COMMAND_OK, "Directory successfully changed");
                            send(clientSocket, response.c_str(), response.length(), 0);
                        } else {
                            std::string response = "550 Failed to change directory\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                        }
                        break;
                    }
                    
                    case FTPCommand::PASV: {
                        if (!user.authenticated) {
                            std::string response = "530 Please login with USER and PASS\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                            break;
                        }
                        
                        // Close existing passive socket
                        if (passiveSocket != -1) {
                            close(passiveSocket);
                            passiveSocket = -1;
                        }
                        
                        try {
                            passiveSocket = ConnectionManager::createPassiveSocket(dataPort);
                            
                            std::string passiveResponse = FTPProtocol::getResponse(
                                FTPResponseCode::ENTERING_PASSIVE_MODE, 
                                "Entering Passive Mode (" + 
                                ConnectionManager::formatPassiveResponse(clientIP, dataPort) + ")");
                            send(clientSocket, passiveResponse.c_str(), 
                                 passiveResponse.length(), 0);
                            log("PASV: " + clientIP + ":" + std::to_string(dataPort));
                        } catch (const std::exception& e) {
                            log("Error creating passive socket: " + std::string(e.what()));
                            std::string response = "425 Can't open data connection\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                        }
                        break;
                    }
                    
                    case FTPCommand::PORT: {
                        std::string response = "500 PORT mode not supported, use PASV\r\n";
                        send(clientSocket, response.c_str(), response.length(), 0);
                        break;
                    }
                    
                    case FTPCommand::EPSV: {
                        std::string response = "500 EPSV not supported, use PASV\r\n";
                        send(clientSocket, response.c_str(), response.length(), 0);
                        break;
                    }
                    
                    case FTPCommand::LIST: {
                        if (!user.authenticated) {
                            std::string response = "530 Please login with USER and PASS\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                            break;
                        }
                        
                        // Auto-enter passive mode if not already
                        if (passiveSocket == -1) {
                            try {
                                passiveSocket = ConnectionManager::createPassiveSocket(dataPort);
                                std::string passiveResponse = FTPProtocol::getResponse(
                                    FTPResponseCode::ENTERING_PASSIVE_MODE,
                                    "Entering Passive Mode (" + 
                                    ConnectionManager::formatPassiveResponse(clientIP, dataPort) + ")");
                                send(clientSocket, passiveResponse.c_str(), 
                                     passiveResponse.length(), 0);
                            } catch (const std::exception& e) {
                                std::string response = "425 Can't open data connection\r\n";
                                send(clientSocket, response.c_str(), response.length(), 0);
                                break;
                            }
                        }
                        
                        std::string response = FTPProtocol::getResponse(
                            FTPResponseCode::FILE_STATUS_OK, "Here comes the directory listing");
                        send(clientSocket, response.c_str(), response.length(), 0);
                        
                        int currentPassive = passiveSocket;
                        passiveSocket = -1;
                        
                        threadPool.submit([this, currentPassive, clientSocket, arg]() -> int {
                            int dataSocket = ConnectionManager::acceptConnection(currentPassive, 10);
                            if (dataSocket < 0) {
                                log("LIST: Failed to accept data connection");
                                close(currentPassive);
                                std::string error = "425 Can't open data connection\r\n";
                                send(clientSocket, error.c_str(), error.length(), 0);
                                return -1;
                            }
                            
                            std::string dirListing = fileHandler.listDirectory(arg);
                            if (dirListing.find("550") == 0) {
                                send(dataSocket, dirListing.c_str(), dirListing.length(), 0);
                            } else {
                                send(dataSocket, dirListing.c_str(), dirListing.length(), 0);
                            }
                            
                            close(dataSocket);
                            close(currentPassive);
                            
                            std::string complete = FTPProtocol::getResponse(
                                FTPResponseCode::TRANSFER_COMPLETE, "Directory send OK");
                            send(clientSocket, complete.c_str(), complete.length(), 0);
                            
                            return 0;
                        }, []() {
                            log("LIST: Transfer completed");
                        });
                        break;
                    }
                    
                    case FTPCommand::RETR: {
                        if (!user.authenticated) {
                            std::string response = "530 Please login with USER and PASS\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                            break;
                        }
                        
                        // Validate file exists
                        fs::path filePath;
                        if (!fileHandler.getFilePath(arg, filePath)) {
                            std::string response = "550 File not found\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                            break;
                        }
                        
                        // Auto-enter passive mode if not already
                        if (passiveSocket == -1) {
                            try {
                                passiveSocket = ConnectionManager::createPassiveSocket(dataPort);
                                std::string passiveResponse = FTPProtocol::getResponse(
                                    FTPResponseCode::ENTERING_PASSIVE_MODE,
                                    "Entering Passive Mode (" + 
                                    ConnectionManager::formatPassiveResponse(clientIP, dataPort) + ")");
                                send(clientSocket, passiveResponse.c_str(), 
                                     passiveResponse.length(), 0);
                            } catch (const std::exception& e) {
                                std::string response = "425 Can't open data connection\r\n";
                                send(clientSocket, response.c_str(), response.length(), 0);
                                break;
                            }
                        }
                        
                        std::string response = FTPProtocol::getResponse(
                            FTPResponseCode::FILE_STATUS_OK, 
                            "Opening BINARY mode data connection for " + arg + 
                            " (" + std::to_string(fs::file_size(filePath)) + " bytes)");
                        send(clientSocket, response.c_str(), response.length(), 0);
                        
                        int currentPassive = passiveSocket;
                        passiveSocket = -1;
                        
                        threadPool.submit([this, currentPassive, clientSocket, arg]() -> int {
                            int dataSocket = ConnectionManager::acceptConnection(currentPassive, 10);
                            if (dataSocket < 0) {
                                log("RETR: Failed to accept data connection");
                                close(currentPassive);
                                std::string error = "425 Can't open data connection\r\n";
                                send(clientSocket, error.c_str(), error.length(), 0);
                                return -1;
                            }
                            
                            std::ifstream file = fileHandler.getFileForReading(arg);
                            if (file.is_open()) {
                                char fileBuffer[65536];
                                while (file.read(fileBuffer, sizeof(fileBuffer)) || 
                                       file.gcount() > 0) {
                                    send(dataSocket, fileBuffer, file.gcount(), 0);
                                }
                                file.close();
                            }
                            
                            close(dataSocket);
                            close(currentPassive);
                            
                            std::string complete = FTPProtocol::getResponse(
                                FTPResponseCode::TRANSFER_COMPLETE, "Transfer complete");
                            send(clientSocket, complete.c_str(), complete.length(), 0);
                            
                            return 0;
                        }, []() {
                            log("RETR: Transfer completed");
                        });
                        break;
                    }
                    
                    case FTPCommand::STOR: {
                        if (!user.authenticated) {
                            std::string response = "530 Please login with USER and PASS\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                            break;
                        }
                        
                        // Auto-enter passive mode if not already
                        if (passiveSocket == -1) {
                            try {
                                passiveSocket = ConnectionManager::createPassiveSocket(dataPort);
                                std::string passiveResponse = FTPProtocol::getResponse(
                                    FTPResponseCode::ENTERING_PASSIVE_MODE,
                                    "Entering Passive Mode (" + 
                                    ConnectionManager::formatPassiveResponse(clientIP, dataPort) + ")");
                                send(clientSocket, passiveResponse.c_str(), 
                                     passiveResponse.length(), 0);
                            } catch (const std::exception& e) {
                                std::string response = "425 Can't open data connection\r\n";
                                send(clientSocket, response.c_str(), response.length(), 0);
                                break;
                            }
                        }
                        
                        std::string response = FTPProtocol::getResponse(
                            FTPResponseCode::FILE_STATUS_OK, 
                            "Opening BINARY mode data connection for " + arg);
                        send(clientSocket, response.c_str(), response.length(), 0);
                        
                        int currentPassive = passiveSocket;
                        passiveSocket = -1;
                        
                        threadPool.submit([this, currentPassive, clientSocket, arg]() -> int {
                            int dataSocket = ConnectionManager::acceptConnection(currentPassive, 10);
                            if (dataSocket < 0) {
                                log("STOR: Failed to accept data connection");
                                close(currentPassive);
                                std::string error = "425 Can't open data connection\r\n";
                                send(clientSocket, error.c_str(), error.length(), 0);
                                return -1;
                            }
                            
                            std::ofstream file = fileHandler.getFileForWriting(arg);
                            if (file.is_open()) {
                                char fileBuffer[65536];
                                int bytesReceived;
                                while ((bytesReceived = recv(dataSocket, fileBuffer, 
                                                             sizeof(fileBuffer), 0)) > 0) {
                                    file.write(fileBuffer, bytesReceived);
                                }
                                file.close();
                            }
                            
                            close(dataSocket);
                            close(currentPassive);
                            
                            std::string complete = FTPProtocol::getResponse(
                                FTPResponseCode::TRANSFER_COMPLETE, "Transfer complete");
                            send(clientSocket, complete.c_str(), complete.length(), 0);
                            
                            return 0;
                        }, []() {
                            log("STOR: Transfer completed");
                        });
                        break;
                    }

                    // 在 switch 中添加这些 case（在 TYPE 和 SYST 之后）

                    case FTPCommand::SIZE: {
                        if (!user.authenticated) {
                            std::string response = "530 Please login with USER and PASS\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                            break;
                        }
                        
                        fs::path filePath;
                        if (fileHandler.getFilePath(arg, filePath)) {
                            auto size = fs::file_size(filePath);
                            std::string response = "213 " + std::to_string(size) + "\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                        } else {
                            std::string response = "550 File not found\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                        }
                        break;
                    }

                    case FTPCommand::MDTM: {
                        if (!user.authenticated) {
                            std::string response = "530 Please login with USER and PASS\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                            break;
                        }
                        
                        fs::path filePath;
                        if (fileHandler.getFilePath(arg, filePath)) {
                            auto ftime = fs::last_write_time(filePath);
                            auto cftime = std::chrono::system_clock::to_time_t(
                                std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                                    ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()));
                            std::stringstream ss;
                            ss << "213 " << std::put_time(std::gmtime(&cftime), "%Y%m%d%H%M%S") << "\r\n";
                            std::string response = ss.str();
                            send(clientSocket, response.c_str(), response.length(), 0);
                        } else {
                            std::string response = "550 File not found\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                        }
                        break;
                    }
                    
                    case FTPCommand::REST: {
                        if (!user.authenticated) {
                            std::string response = "530 Please login with USER and PASS\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                            break;
                        }
                        
                        try {
                            user.restartOffset = std::stoll(arg);
                            std::string response = FTPProtocol::getResponse(
                                FTPResponseCode::COMMAND_OK, 
                                "Restart position accepted (" + arg + ")");
                            send(clientSocket, response.c_str(), response.length(), 0);
                        } catch (...) {
                            std::string response = "501 Invalid restart position\r\n";
                            send(clientSocket, response.c_str(), response.length(), 0);
                        }
                        break;
                    }
                    
                    case FTPCommand::QUIT: {
                        std::string response = FTPProtocol::getResponse(
                            FTPResponseCode::CLOSING_CONTROL, "Goodbye");
                        send(clientSocket, response.c_str(), response.length(), 0);
                        goto cleanup;
                    }
                    
                    default: {
                        std::string response = FTPProtocol::getResponse(
                            FTPResponseCode::NOT_IMPLEMENTED, "Command not implemented");
                        send(clientSocket, response.c_str(), response.length(), 0);
                        break;
                    }
                }
            }
        }
        
    cleanup:
        if (passiveSocket != -1) {
            close(passiveSocket);
        }
        close(clientSocket);
        log("Client handler finished");
        
    } catch (const std::exception& e) {
        log("Error handling client: " + std::string(e.what()));
        if (passiveSocket != -1) close(passiveSocket);
        if (clientSocket != -1) close(clientSocket);
    }
}

void FTPServer::start() {
    running = true;
    int serverSocket = -1;
    
    try {
        serverSocket = ConnectionManager::createServerSocket(controlPort);
        
        log("FTP Server started on port " + std::to_string(controlPort));
        log("Server IP: " + ConnectionManager::getLocalIP());
        
        while (running) {
            int clientSocket = ConnectionManager::acceptConnection(serverSocket, 5);
            if (clientSocket < 0) {
                if (running) {
                    log("Accept timeout, retrying...");
                }
                continue;
            }
            
            log("New client connected");
            
            threadPool.submit([this, clientSocket]() -> int {
                handleClient(clientSocket);
                return 0;
            }, []() {
                log("Client handler completed");
            });
        }
        
    } catch (const std::exception& e) {
        log("Server error: " + std::string(e.what()));
    }
    
    if (serverSocket != -1) {
        close(serverSocket);
    }
}

void FTPServer::stop() {
    running = false;
}