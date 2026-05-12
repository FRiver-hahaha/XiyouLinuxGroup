#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <stdexcept>

class connectMannger {
public:

    static int createSocket(const int port) {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if(serverSocket < 0) {
            throw std::runtime_error("无法创建套接字");
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = 0;// 随机端口
        serverAddr.sin_addr.s_addr = INADDR_ANY;// 随机ip地址

        if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            close(serverSocket);
            throw std::runtime_error("无法绑定套接字和ip地址");
        }

        if(listen(serverSocket, 4) < 0) {
            close(serverSocket);
            throw std::runtime_error("无法监听");
        }


        // 创建套接字成功
        return serverSocket;
    }

    static int acceptConnection(const int serverSocket, int timeSeconds = 30) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        memset(&clientAddr, 0, clientLen);
        
        struct timeval tv;
        tv.tv_sec = timeSeconds;
        tv.tv_usec = 0;
        setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
        
        return accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    }
}