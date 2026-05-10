#include "simple_ftp_client.h"

SimpleFTPClient::SimpleFTPClient() : controlSocket(-1) {}

SimpleFTPClient::~SimpleFTPClient() {
    disconnect();
}

bool SimpleFTPClient::connect(const std::string& server, int port) {
    controlSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (controlSocket < 0) return false;
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server.c_str(), &serverAddr.sin_addr) <= 0) {
        close(controlSocket);
        controlSocket = -1;
        return false;
    }
    
    if (::connect(controlSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(controlSocket);
        controlSocket = -1;
        return false;
    }
    
    return true;
}

std::string SimpleFTPClient::sendCommand(const std::string& command) {
    if (controlSocket == -1) {
        return "Not connected";
    }
    
    std::string cmd = command + "\r\n";
    if (send(controlSocket, cmd.c_str(), cmd.length(), 0) < 0) {
        return "Send failed";
    }
    
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(controlSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRead <= 0) {
        return "Receive failed";
    }
    
    return std::string(buffer);
}

void SimpleFTPClient::disconnect() {
    if (controlSocket != -1) {
        close(controlSocket);
        controlSocket = -1;
    }
}