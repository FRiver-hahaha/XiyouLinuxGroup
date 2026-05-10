#ifndef SIMPLE_FTP_CLIENT_H
#define SIMPLE_FTP_CLIENT_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class SimpleFTPClient {
private:
    int controlSocket;
    
public:
    SimpleFTPClient();
    ~SimpleFTPClient();
    
    bool connect(const std::string& server, int port);
    std::string sendCommand(const std::string& command);
    void disconnect();
};

#endif // SIMPLE_FTP_CLIENT_H