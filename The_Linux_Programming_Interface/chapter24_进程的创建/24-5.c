#include "tlpi_hdr.h"

int main(int argc, char* argv[]) {
    int numChildren, j;
    pid_t childPid;

    if(argc > 1 && strcmp(argv[1], "--help") == 0) {
        usageErr("$s [num-children]\n", argv[0]);
    }

    numChildren = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-children") : 1;

    setbuf(stdout, NULL);// 禁用标准输出缓冲

    for(j = 0; j < numChildren; ++j) {
        switch (childPid = fork())
        {
        case -1:
            errExit("fork");
            break;
        
        case 0:
            printf("%d child\n", j);
            _exit(EXIT_SUCCESS);

        default:
            printf("%d parent\n", j);
            wait(NULL);// 等待子进程终止
            break;
        }
    }

    exit(EXIT_SUCCESS);
}