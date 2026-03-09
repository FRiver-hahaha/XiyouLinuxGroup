#define _GNU_SOURCE// 用来导入GNU扩展，使得中文内容编码正常
#include <fcntl.h>
#include <unistd.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>// 等待子进程
#include <string.h>// 字符串比较
#include <signal.h>// 中断 ctrl + c 信号

#define COLOR_RESET "\033[0m" // 重置 
#define COLOR_WELCOME "\033[1;34m" // 欢迎界面(粗体蓝色)
#define COLOR_POINT "\033[2;32m" // 高亮显示当前行指向(粗体暗绿色)
#define COLOR_HIGHLIGHT "\033[1;32m" // 高亮显示当前行索引(粗体亮绿色)
#define MAX_PATH 256
#define MAX_ARGS 64
#define MAX_BGPROCESS 128
#define MAX_PIPES 16


typedef struct {
    pid_t pid;
    char command[256];
    int isstillhere;
}Bgprocess;

typedef struct {
    char *args[MAX_ARGS];
    int argc;
    char *inputFile;
    char *outputFile;
    int appendOutput;  // 1代表>>,0代表>
    int hasPipe;
    int pipeFd[2];
} Command;

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
void CleanCommand(Command commands[]);// 清理管道

/*
以下三个函数先设置command结构体，之后设置重定向，最后创建进程执行管道
*/ 

int HandlePipe(Command commands[], char command[], char* args[], int* isback);// 处理管道
int setRediraction(Command commands[]);// 设置重定向
void execPipe(Command commands[], int cmdCount, int isback);// 执行管道

int main() {
    // 注册信号
    signal(SIGINT, SIG_IGN);// 解决ctrl + c中断进程的问题
    signal(SIGQUIT, SIG_IGN);// 解决ctrl + \中断进程的问题
    signal(SIGCHLD, SignalZombie);// 处理僵尸进程

    setvbuf(stdin, NULL, _IOLBF, 0);// 处理行缓冲，使得快速显示内容
    setvbuf(stdout, NULL, _IOLBF, 0);

    FirstShow();//启动整体程序
    exit(EXIT_SUCCESS);
}

/*
    清理命令资源,
    不需要清理文件名字符串，
    因为它作为指针指向原始命令字符串
*/

void CleanCommand(Command commands[]) {
    commands->argc = 0;
    commands->inputFile = NULL;
    commands->outputFile = NULL;
    commands->appendOutput = 0;
    commands->hasPipe = 0;
}

/*
    用来执行管道。
    先设置好前置管道和当前管道，
    在具有一定管道数目内，
    执行for循环创建进程。
    在子进程内，
    设置好输入输出重定向。
    之后执行进程。
    在父进程内，
    关闭前置进程，并且设置好当前进程。
    由于当前进程下的管道属于同一个进程组，同时执行，
    所以可以只记录第一个Pipepid。

*/

void execPipe(Command commands[], int cmdCount, int isback) {
    int prePipe[2] = {-1, -1};// 分别为读端，写端
    int currentPipe[2];// 分别为读端，写端
    pid_t PipePids[MAX_PIPES];

    for(int i = 0; i < cmdCount; ++i) {
        if(i < cmdCount - 1) {
            pipe(currentPipe);
        }

        switch (PipePids[i] = fork())
        {
        case -1:
            perror("fork");
            break;
            
        case 0:
            if(isback) {// 忽略ctrl + c 和 ctrl + \ 的信号
                signal(SIGINT, SIG_IGN);
                signal(SIGQUIT, SIG_IGN);
            }

            if(prePipe[0] != -1) {// 设置输入重定向
                dup2(prePipe[0], STDIN_FILENO);
                close(prePipe[0]);
                close(prePipe[1]);
            }

            if(i < cmdCount - 1) {// 设置输出重定向
                close(currentPipe[0]);
                dup2(currentPipe[1], STDOUT_FILENO);
                close(currentPipe[1]);
            }

            if(setRediraction(commands + i)) {
                exit(1);
            }
            
            for(int fd = 3; fd < 1024; ++fd) {// 关闭了
                if(fd != STDERR_FILENO && fd != STDIN_FILENO && fd != STDOUT_FILENO) {
                    close(fd);
                }
            }

            char* execPath = SearchPath(commands[i].args[0]);
            if(!execPath) {
                fprintf(stderr, "未找到该命令：%s\n", commands[i].args[0]);
                exit(127);
            }

            extern char** environ;// 不可写成* environ[]的形式，这与_GNU_SOURCE中声明的冲突
            if(execve(execPath, commands[i].args, environ) == -1) {
                fprintf(stderr, "execve失败\n");
                exit(126);
            }
            break;
        
        default:
            if(prePipe[0] != -1) {// 关闭上一个管道的fd，否则不会结束。
                close(prePipe[0]);
                close(prePipe[1]);
            }

            if(i < cmdCount - 1) {// 保存当前管道
                prePipe[0] = currentPipe[0];
                prePipe[1] = currentPipe[1];
            }
            break;
        }
    }

    if(isback) {
        char cmd[256] = "";// 后台创建进程。
        for(int i = 0; i < cmdCount; ++i) {
            if(i > 0) {
                strcat(cmd, "|");
            }
            strcat(cmd, commands[i].args[0]);
        }
        AddBgProcess(PipePids[0], cmd);
    }else {
        // 前台等待所有进程。
        for(int i = 0; i < cmdCount; ++i) {
            waitpid(PipePids[i], NULL, 0);
        }
    }
}

/*
    用来针对管道设置重定向。
    首先根据cmd结构体内的参数，
    决定是否要进行重定向。
    处理时要新开一个fd，
    即获取到当前文件，
    之后用dup2将重定向的宏，
    指定到当前文件，并且关闭之前的文件描述符fd。
    如果出错，返回-1,
    正常则返回。
*/

int setRediraction(Command commands[]) {
    if(commands->inputFile) {// 处理输入重定向
        int fd = open(commands->inputFile, O_RDONLY);
        if(fd < 0) {
            fprintf(stderr, "无法打开该输入文件：%s\n", commands->inputFile);
            return -1;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if(commands->outputFile) {
        int flags = O_WRONLY | O_CREAT;
        if(commands->appendOutput) {
            flags |= O_APPEND;
        }else {
            flags |= O_TRUNC;
        }

        int fd = open(commands->outputFile, flags, 0644);// 644 代表 所有者读写，所属组只读，其他用户只读
        if(fd < 0) {
            fprintf(stderr, "无法打开该输出文件：%s\n", commands->outputFile);
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    return 0;
}

/*
    用来处理命令参数中的管道。
    先在一个for循环内记录非管道参数的数目。
    之后根据这个数目，
    创建一个for循环，
    在数目内设置每个command结构体的参数。
    先初始化好每个参数，
    之后根据<,>,>>，分别对每个结构体cmd内进行标记。
    如果if都被跳过了，则把当前参数标记好。
*/

int HandlePipe(Command commands[], char command[], char* args[], int* isback) {
    int countNoPipe = 0;
    int argIndex = 0;
    CleanCommand(&commands[countNoPipe]);

    while(args[argIndex]) {
        if(strcmp(args[argIndex], "|") == 0) {//遇到管道则跳过处理
            commands[countNoPipe].args[commands[countNoPipe].argc] = NULL;
            countNoPipe++;
            CleanCommand(&commands[countNoPipe]);
            argIndex++;
            continue;
        }

        // 处理参数成为管道，对每个管道开始设置标记，供后续函数进行识别
        if(strcmp("<", args[argIndex]) == 0) {
            commands[countNoPipe].inputFile = args[++argIndex];
        }else if(strcmp(">", args[argIndex]) == 0) {
            commands[countNoPipe].outputFile = args[++argIndex];
            commands[countNoPipe].appendOutput = 0;
        }else if(strcmp(">>", args[argIndex]) == 0) {
            commands[countNoPipe].outputFile = args[++argIndex];
            commands[countNoPipe].appendOutput = 1;
        }else {
            commands[countNoPipe].args[commands[countNoPipe].argc++] = args[argIndex];
        }
        argIndex++;
    }

    commands[countNoPipe].args[commands[countNoPipe].argc] = NULL;
    commands[countNoPipe].hasPipe = 0;
    return countNoPipe + 1;// 返回命令数量
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
        if(access(command, X_OK) == 0) {// 绝对路径直接返回
            strncpy(fullpath, command, MAX_PATH - 1);
            fullpath[MAX_PATH - 1] = '\0';
            free(pathcopy);
            return fullpath;
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

    char* token;
    char* saveptr;
    char* p = start;
    int inQuote = 0;
    char quoteChar = 0;

    while(cnt < MAX_ARGS - 1 && *p) {
        //除杂
        while(*p == ' ' || *p == '\t') p++;
        if(*p == '\0') break;
        
        if(*p == '"' || *p == '\'') {
            inQuote = 1;
            quoteChar = *p;//确定为引号，供后续比较，确定区间
            p++; // 跳过左引号
            token = p;
            
            while(*p) {
                if(*p == quoteChar) {
                    // 如果是引号，检查前面是否是转义符
                    if(*(p-1) != '\\') {
                        break;  // 找到未被转义的右引号
                    }
                }
                p++;
            }
            
            if(*p == quoteChar) {//找到右引号
                *p = '\0';
                args[cnt++] = token;
                p++;
            }
            inQuote = 0;
        } else {
            // 普通参数
            token = p;
            while(*p && *p != ' ' && *p != '\t') p++;//除杂
            if(*p) {
                *p = '\0';
                p++;
            }
            args[cnt++] = token;
        }
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
            printf("%s->%s%s%s%s ", COLOR_POINT, COLOR_RESET, COLOR_HIGHLIGHT, currentPath, COLOR_RESET);
        }else {
            printf("%s->%s ", COLOR_POINT, COLOR_RESET);
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
                char targetPath[MAX_PATH];// 保存具有根目录的完整路径节点，供未来遇到没有完整路径的情况时，可以拿来与其拼接

                if(args[1][0] == '/') {
                    strncat(targetPath, args[1], MAX_PATH - 1);
                    targetPath[MAX_PATH - 1] = '\0';
                }else {
                    if(getcwd(targetPath, MAX_PATH)) {
                        strcpy(prevPath, targetPath);
                        strncat(targetPath, "/", MAX_PATH - strlen(targetPath) - 1);
                        strncat(targetPath, args[1], MAX_PATH - strlen(targetPath) - 1);
                    }
                }
                chdir(args[1]);
            }
        }

        if(strchr(command, '|') != NULL || strchr(command, '>') != NULL || strchr(command, '<') != NULL) {
            Command commands[MAX_PIPES];
            ParseCommand(command, args, &isback);
            int cmdCount = HandlePipe(commands, command, args, &isback);

            if(cmdCount) {
                execPipe(commands, cmdCount, isback);
            }
        }else {
            ParseCommand(command, args, &isback);// 解析命令行


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
                extern char** environ;// 不可写成* environ[]的形式，这与_GNU_SOURCE中声明的冲突
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
}   