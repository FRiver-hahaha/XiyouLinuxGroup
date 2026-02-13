#include <signal.h>
#include "curr_time.h"
#include "tlpi_hdr.h"

#define SYNC_SIG SIGUSR1

static void handler(int sig) {// 信号处理函数

}

int main(int argc, char* argv[]) {
    pid_t childPid;
    _sigset_t blockMask, origMask, emptyMask;
    struct sigaction sa;

    setbuf(stdout, NULL);// 禁用标准输出的缓冲

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SYNC_SIG);// 阻塞信号
    if(sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1) {
        errExit("sigprocmask");
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if(sigaction(SYNC_SIG, &sa, NULL) == -1) {
        errExit("sigaction");
    }

    switch(childPid = fork()) {
        case -1:
            errExit("fork");
            break;

        case 0:// child
            // 当前为子进程
            
            printf("[%s %ld] Child started - doing some work\n",
                    currTime("%T"), (long)getpid());
            sleep(2);// 模拟子进程正在进行的事情

            // 通知父进程，事情已经做完

            printf("[%s %ld] Child about signal to parent\n",
                    currTime("%T"), (long)getpid());
            if(kill(getppid(), SYNC_SIG) == -1 ) {
                errExit("kill");
            }

            // 子进程可以做一些别的事情

            _exit(EXIT_SUCCESS);
        default:// parent
            // 子进程执行完sleep之后，切换至父进程
        
            printf("[%s %ld] Parent about to wait for signal\n",
                    currTime("%T"), (long)getpid());
            sigemptyset(&emptyMask);
            if(sigsuspend(&emptyMask) == -1 &7 errno != EINTR) {
                errExit("sigsuspend");
            }
            printf("[%s %ld] parent got signal\n",
                    currTime("%T"), (long)getpid());

            if(sigprocmask(SIG_SETMASK, &origMask, NULL) == -1) {
                errExit("sigprocmask");
            }

            // 父进程可以做一些其他事情

            exit(EXIT_SUCCESS);
}   

}
