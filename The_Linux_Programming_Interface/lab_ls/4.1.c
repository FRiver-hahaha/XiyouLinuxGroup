#include<stdlib.h>//库函数

//系统调用库函数
#include<sys/stat.h>
#include<fcntl.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char* argv[]) {
    int inputfd, outputfd, openflags;//文件标识符
    mode_t filePerms;//文件权限
    __ssize_t numRead;
    char buf[BUF_SIZE];

    if(argc != 3 || strcmp(argv[1], "--help") == 0) {
        usagErr("%s old-file new-file\n",argv[0]);
    }

    inputfd = open(argv[1], O_RDONLY);//打开输入文件
    if(inputfd == -1) {
        errExit("opening file %s",argv[2]);
    }

    openflags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    outputfd = open(argv[2], openflags, filePerms);//打开输出文件
    if(outputfd == -1) {
        errExit("opening file %s",argv[2]);
    }

    //读写文件

    while((numRead = read(inputfd, buf, BUF_SIZE)) > 0) {
        if((outputfd,buf,numRead) != numRead) {
            fatal("could't write whole buffer.");
        }
    }
    if(numRead == -1) {
        errExit("read");
    }
    if(close(inputfd) == -1) {
        errExit("close inputFile");
    }
    if(close(outputfd) == -1) {
        errExit("close outputFile");
    }

    exit(EXIT_SUCCESS);
} 