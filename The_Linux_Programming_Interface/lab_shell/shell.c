// 3.4已完成cd切换

// 明天完成sort
#include <locale.h>
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
#define MAX_BGPROCESS 128


typedef struct {
    pid_t pid;
    char command[256];
    int isstillhere;
}Bgprocess;

Bgprocess bgProcess[MAX_BGPROCESS];// 后台进程
int bgpCount = 0;// 后台进程数


void FirstShow();
void Shell();
void Error(int isError);
int ParseCommand(char* command, char* args[], int* isback);// 解析命令行，将token放入参数指针数组
char* SearchPath(char* command);// 路径搜索
void SignalZombie(int sig);  // 处理僵尸进程
void AddBgProcess(pid_t pid, char* command);// 添加后台进程
void CheckBgProcess(void);// 检查后台进程

int main() {
    setlocale(LC_ALL, "");// 使得shell支持更多编码

    signal(SIGINT, SIG_IGN);// 解决ctrl + c中断进程的问题
    signal(SIGCHLD, SignalZombie);// 处理僵尸进程

    setvbuf(stdin, NULL, _IOLBF, 0);// 处理行缓冲，使得快速显示内容
    setvbuf(stdout, NULL, _IOLBF, 0);

    FirstShow();//启动整体程序
    exit(EXIT_SUCCESS);
}

/*
    用来移除已经结束的进程。
    在while循环内作为检查的起始端。
    在for循环中利用冒泡排序，
    依次将整体移动，清除0位。
    如果碰到了正在运行的进程则跳过，i++
    否则进行for循环，并且减少bgpCount。
*/

void CheckBgProcess() {
    int i = 0;
    while(i < bgpCount) {
        if(!bgProcess[i].isstillhere) {
            for(int j = i; j < bgpCount - 1; j++) {
                bgProcess[j] = bgProcess[j + 1];
            }
            bgpCount--;
        }else {
            i++;
        }
    }
}

/*
    用来添加后台进程到后台进程结构体数组内。
    如果当前后台进程数少于最大进程数，
    则创建好后台进程，
    并打印好创建信息。
*/

void AddBgProcess(pid_t pid, char* command) {
    if(bgpCount < MAX_BGPROCESS) {
        bgProcess[bgpCount].pid = pid;
        bgProcess[bgpCount].isstillhere = 1;
        memcpy(bgProcess[bgpCount].command, command, 255);
        bgProcess[bgpCount].command[255] = '\0';

        printf("[ %d ] %d\n", ++bgpCount, pid);// 打印后台进程数目，进程pid
    }
}

/*
    用来处理僵尸进程。
    获取到当前进程的状态和pid，
    在while循环内通过wait no hang(不挂起等待)的方式，
    回收每个僵尸进程，
    设置好回收状态。
    并且提示给用户进程的回收状态。
*/

void SignalZombie(int sig) {
    int status;
    pid_t currentPid;

    while((currentPid = waitpid(-1, &status, WNOHANG)) > 0) {
        for(int i = 0; i < bgpCount; ++i) {
            if(bgProcess[i].pid == currentPid && bgProcess[i].isstillhere) {
                bgProcess[i].isstillhere = 0;// 设置为已回收
                printf("\n[ %d ] %s 进程已回收\n", currentPid, bgProcess[i].command);
                fflush(stdout);
                break;
            }
        }
    }
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

    if(!command || *command == '\0') {
        return NULL;
    }


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


    // 处理冗余内容
    char* start = command;
    while(*start == ' ' || *start == '\t') start++;

    char* end = command + len - 1;
    while(end > start && (*end == ' ' || *end == '\t' || *end == '\n')) {
        *end = '\0';
        end--;
    }

    if(end >= start && *end == '&') {
        *isback = 1;
        *end = '\0';

        end--;
        while(end >= start && (*end == ' ' || *end == '\t')) {
            *end = '\0';
            end--;
        }
    }
    // 处理冗余内容

    if(*start == '\0') {
        args[0] = NULL;
        return 0;
    }

    char* token = strtok(start, " \t");

    while(cnt <= MAX_ARGS && token != NULL) {
        args[cnt++] = token;
        token = strtok(NULL, " \t");
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
    
    case 2:// 未获取到环境变量
        printf("\n未获取到环境变量...\n");
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
    char flag = '#';
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

    char command[256];
    char currentPath[MAX_PATH];
    char answer[10];
    char* execpath = NULL;
    pid_t pidChild;
    int status, isback = 0;

    while(1) {

        static char prevPath[MAX_PATH] = "";

        CheckBgProcess();

        if(getcwd(currentPath, MAX_PATH)) {
            printf("->%s ", currentPath);
        }else {
            printf("-> ");
        }

        fflush(stdout);

        if(!fgets(command, 256, stdin)) {// 输入命令
            if(feof(stdin)) {// 如果遇到eof(ctrl + D)
                printf("\n");
                Error(0);
                exit(EXIT_SUCCESS);
            }
            continue;
        }

        size_t len = strlen(command);
        if (len > 0 && command[len - 1] == '\n') {
            command[len - 1] = '\0';
            len--;
        }

        if(strcmp("exit", command) == 0 || strcmp("quit", command) == 0) {// 处理退出
            if(bgpCount > 0) {
                printf("当前后台还有 %d 个进程正在运行，是否要强行关闭? (y/n): ", bgpCount);
                if(fgets(answer, sizeof(answer), stdin)) {
                    if(answer[0] != 'y' && answer[0] != 'Y') {
                        continue;
                    }
                }
            }
            Error(0);
            exit(EXIT_SUCCESS); 
        }

        // 添加jobs命令，显示后台任务
        if (strcmp("jobs", command) == 0) {
            if (bgpCount == 0) {
                printf("没有后台进程\n");
            } else {
                printf("后台进程列表：\n");
                for (int i = 0; i < bgpCount; i++) {
                    printf("[%d] %d\t%s\t%s\n", 
                           i + 1, 
                           bgProcess[i].pid, 
                           bgProcess[i].isstillhere ? "运行中" : "已完成",
                           bgProcess[i].command);
                }
            }
            continue;
        }
        if(strncmp("cd", command, 2) == 0 && (command[2] == ' ' || command[2] == '\0')) {// 处理cd
            ParseCommand(command, args, &isback);// 解析命令行

            /*
                切换回主目录
                先获取到环境变量home，
                如果没有就报错，有就切过去
            */

            if(args[1] == NULL || strcmp(args[1], "~") == 0) {
                char* home = getenv("HOME");
                if(!home) {
                    Error(2);
                }else {
                    getcwd(prevPath, MAX_PATH);
                    chdir(home);
                }
            }

            /*
                切换回上一级目录
                先创建一个静态数组，
                用来存储上一级目录，默认为空。
                如果上一级目录没内容就报错，
                有内容则先把当前目录保存下来，
                在切换回去之前，
                把当前目录保存为上一级目录，之后切换目录
            */

            else if (strcmp(args[1], "-") == 0) {// 切换回上一个目录
                if (prevPath[0] == '\0') {
                    getcwd(prevPath, MAX_PATH);
                    continue;
                } else {
                    char tmpCurrentPath[MAX_PATH];
                    if (getcwd(tmpCurrentPath, MAX_PATH)) {
                        chdir(prevPath);
                        printf("cd: 已切换至%s\n", prevPath);
                        strcpy(prevPath, tmpCurrentPath);
                        
                        
                    }
                }

             /*
                切换回上一级目录
                先创建一个静态数组，
                用来存储上一级目录，默认为空。
                如果上一级目录没内容就报错，
                有内容则先把当前目录保存下来，
                在切换回去之前，
                把当前目录保存为上一级目录，之后切换目录
            */

            }else {
                char targetPath[MAX_PATH];

                if(args[1][0] == '/') {
                    strncat(targetPath, args[1], MAX_PATH - 1);
                    targetPath[MAX_PATH - 1] = '\0';
                }else {
                    if(getcwd(targetPath, MAX_PATH)) {
                        strcpy(prevPath
                , targetPath);
                        strncat(targetPath, "/", MAX_PATH - strlen(targetPath) - 1);
                        strncat(targetPath, args[1], MAX_PATH - strlen(targetPath) - 1);
                    }
                }

                chdir(args[1]);
            }
        }else {
            ParseCommand(command, args, &isback);// 解析命令行
        }

        execpath = SearchPath(args[0]);

        switch (pidChild = fork())
        {
        case -1:
            perror("fork");
            break;
        
        case 0:
            // 如果是后台进程，忽略终端信号
            if (isback) {
                signal(SIGINT, SIG_IGN);
                signal(SIGQUIT, SIG_IGN);
            }
            extern char* environ[];
            if(execve(execpath, args, environ) == -1) {
                exit(1);
            }
            break;
        default:
            if(isback) {
                AddBgProcess(pidChild, command);
            }else {
                waitpid(pidChild, &status, 0);
            }
            break;
        }
    }
}   