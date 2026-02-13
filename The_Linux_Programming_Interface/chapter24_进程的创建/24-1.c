#include "tlpi_hdr.h"

/* 这是一个创建进程的程序实例，检验了fork()的功能 */

static int idata = 111;

int main(int argc, char* argv[]) {
    int istack = 222;
    pid_t childpid;

    switch (childpid = fork())// 习惯写法，多路分支
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);        

    case 0:// 父子进程同时进行
        idata *= 3;
        istack *= 3;
        // sleep(3); 可验证没有wait()的情况
        break;
    
    default:
        sleep(3);
        break;
    }

    printf("PID = %ld %s idata = %d istack = %d\n",(long)getpid(), 
            (childpid == 0) ? "(child)" : "(parent)", idata, istack);

    exit(EXIT_SUCCESS);
}