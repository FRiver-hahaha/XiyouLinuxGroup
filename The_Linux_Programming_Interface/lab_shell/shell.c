// 3.2已完成路径搜索

// 明天完成后台运行

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

void FirstShow();
void Shell();
void Error(int isError);
int ParseCommand(char* command, char* args[], int* isback);// 解析命令行，将token放入参数指针数组
char* SearchPath(char* command);// 路径搜索

int main() {
    signal(SIGINT, SIG_IGN);// 解决ctrl + c中断进程的问题
    FirstShow();//启动整体程序
    exit(EXIT_SUCCESS);
}

/*
    用来进行路径搜索。
    先获取到原本路径，
    后对原本路径复制，用复制样本进行拆分，
    把每个目录放成完整路径。
    如果绝对路径正确，则返回绝对路径，
    否则从环境变量里找到并返回完整路径。
    如果什么都没找到，返回NULL。
*/

char* SearchPath(char* command) {
    char* path = getenv("PATH");
    char *pathcopy = strdup(path); 
    char* dir = strtok(pathcopy, ":");
    static char fullpath[MAX_PATH];

    if(command[0] == '/' || command[0] == '.') {
        if(access(command, X_OK) == 0) {// 绝对路径直接返回command
            free(pathcopy);
            return command;
        }
        free(pathcopy);
        return NULL;
    }

    while(dir) {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, command);

        if(access(fullpath, X_OK) == 0) {// 非绝对路径返回完整路径
            free(pathcopy);
            return fullpath;
        }

        dir = strtok(NULL, ":");
    }
    free(pathcopy);
    return NULL;
}

/*
    用来解析从用户获取到的命令行，
    拆分成token放入args参数数组。
    在while循环内拆分token，并逐个放入数组内。
*/

int ParseCommand(char* command, char* args[], int* isback) {
    int cnt = 0, len = strlen(command);
    
    *isback = 0;
    if(len > 0 && command[len - 1] == '&') {
        *isback = 1;
        command[len - 1] = '\0';
        
        while(len > 1 && command[len - 2] == ' ') {
            command[len-- - 2] = '\0';
        }
    }

    char* token = strtok(command, "\t");

    while(cnt <= MAX_ARGS && token != NULL) {
        args[cnt++] = token;
        token = strtok(NULL, "\t");
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
        printf("\n退出shell\n");
        break;

    case 1:// 路径错误
        printf("\n找不到该路径，也可能路径输入错误，请重试...\n");
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

void FirstShow() {
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
    Shell();
}

/*
    shell的主体函数。
    用command数组存储路径。
    fork出子进程之后，把在子进程内部执行execve。
    父进程等待子进程结束之后，继续保持shell状态，并且有Error错误判断。
*/

void Shell() {
    char *args[MAX_ARGS];
    char command[255];
    char* execpath = NULL;
    pid_t pidChild;
    int count = 0, status, isback = 0;
    while(1) {
        write(1, "#  ", 3);
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

        ParseCommand(command, args, &isback);// 解析命令行

        execpath = SearchPath(args[0]);
        
        if(!execpath) {
            Error(1);
            continue;
        }

        switch (pidChild = fork())
        {
        case -1:
            perror("fork");
            break;
        
        case 0:
            extern char* environ[];
            if(execve(execpath, args, environ) == -1) {
                exit(1);
            }
            break;
        default:
            if(!isback) {
                wait(&status);

                if(WIFEXITED(status) && WEXITSTATUS(status) == 1) {
                    Error(1);
                }
            }else {
                printf("\nshell: finished.\n");
            }
            break;
        }
    }
}