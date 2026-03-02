// 3.2

// 明天完成路径搜索

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>// 等待子进程
#include <string.h>// 字符串比较
#include <signal.h>// 中断 ctrl + c 信号

#define COLOR_RESET "\033[0m" // 重置 
#define COLOR_WELCOME "\033[1;34m" // 欢迎界面(粗体蓝色)
#define MAX_PATH 256
#define MAX_ARGS 64

void FirstShow(int argc, char* argv[]);
void Shell(int argc, char* argv[]);
void Error(int isError);
int ParseCommand(char* command, char* args[]);// 解析命令行，将token放入参数指针数组

int main(int argc, char* argv[]) {
    signal(SIGINT, SIG_IGN);// 解决ctrl + c中断进程的问题
    FirstShow(argc, argv);//启动整体程序
    exit(EXIT_SUCCESS);
}

/*
    用来解析从用户获取到的命令行，
    拆分成token放入args参数数组。
    在while循环内拆分token，并逐个放入数组内。
*/

int ParseCommand(char* command, char* args[]) {
    int cnt = 0;
    char* token = strtok(command, '\t');

    while(cnt <= MAX_ARGS && token != NULL) {
        args[cnt++] = token;
        token = strtok(command, '\t');
    }
    args[cnt] = NULL;

    return cnt;
}

/*
    用来判断用户可能出现的错误。
    用isError存储错误参数，
    函数接受参数后执行对应错误码，给用户提示。
*/

void Error(int isError) {
    switch (isError) 
    {
    case 0:// 退出shell
        printf("退出shell\n");
        break;

    case 1:// 路径错误
        printf("找不到该路径，也可能路径输入错误，请重试...\n");
        break;
    
    default:
        break;
    }
}

/*
    shell进入的欢迎界面。
    用了2个字符串指针，之后在两层for循环中，打印界面。
    最后执行shell函数，进入shell内部。
*/

void FirstShow(int argc, char* argv[]) {
    int time;
    char ch, flag = '#';
    char* wl = "Welcome";
    char* wl2 = "Shell";
    for(time = 1; time <= 10; time++) {
        for(int i = 0; i < 30; i++) {
            printf("%c",flag);
            if((i == 29)) {
                printf("\n");
            }
        }
        if(time == 10 / 2) {
            for(int i = 0; i < 10; i++) {
                printf("%c",flag);
            }
            printf("%s%s%s", COLOR_WELCOME, wl, COLOR_RESET);
            for(int i = 0; i < 13; i++) {
                printf("%c",flag);
                if(i == 12) {
                    printf("\n");
                }
            }
            for(int i = 0; i < 11; i++) {
                printf("%c",flag);
            }
            printf("%s%s%s", COLOR_WELCOME, wl2, COLOR_RESET);
            for(int i = 0; i < 14; i++) {
                printf("%c",flag);
                if(i == 13) {
                    printf("\n");
                }
            }
        }
        usleep(50000);
    };
    Shell(argc, argv);
}

/*
    shell的主体函数。
    用command数组存储路径。
    fork出子进程之后，把在子进程内部执行execve。
    父进程等待子进程结束之后，继续保持shell状态，并且有Error错误判断。
*/

void Shell(int argc, char* argv[]) {
    char *args[MAX_ARGS];
    char command[255];
    pid_t pidChild;
    int count = 0, status;
    while(1) {
        write(1, "# ", 2);
        count = read(0, command, 255);

        if(!count) {
            Error(0);
            exit(EXIT_SUCCESS);    
        }

        if(count <= 1) {
            continue;
        }

        command[count - 1] = '\0';// 去除之后的\n，让其形成完整路径

        if(strcmp("exit", command) == 0) {
            Error(0);
            exit(EXIT_SUCCESS);  
        }

        ParseCommand(command, args);// 解析命令行

        switch (pidChild = fork())
        {
        case -1:
            perror("fork");
            break;
        
        case 0:
            extern char* environ[];
            if(execve(command, argv, environ) == -1) {
                exit(1);
            }
            break;
        default:
            wait(&status);

            if(WIFEXITED(status) && WEXITSTATUS(status) == 1) {
                Error(1);
            }
            break;

        }
    }
}
