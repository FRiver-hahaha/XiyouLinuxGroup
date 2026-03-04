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
#define MAX_BG_PROCESSES 128  // 最大后台进程数

void FirstShow();
void Shell();
void Error(int isError);
int ParseCommand(char* command, char* args[], int* isback);// 解析命令行，将token放入参数指针数组
char* SearchPath(char* command);// 路径搜索
void HandleSigChld(int sig);  // SIGCHLD信号处理函数
void AddBgProcess(pid_t pid, char* command);  // 添加后台进程
void CheckBgProcesses();  // 检查后台进程状态

// 后台进程结构体
typedef struct {
    pid_t pid;
    char command[256];
    int active;  // 1表示活跃，0表示已结束
} BgProcess;

BgProcess bg_processes[MAX_BG_PROCESSES];
int bg_count = 0;

int main() {
    signal(SIGINT, SIG_IGN);// 解决ctrl + c中断进程的问题
    signal(SIGCHLD, HandleSigChld);  // 处理子进程退出信号
    
    // 设置stdin为行缓冲
    setvbuf(stdin, NULL, _IOLBF, 0);
    setvbuf(stdout, NULL, _IOLBF, 0);
    
    FirstShow();//启动整体程序
    exit(EXIT_SUCCESS);
}

/*
    SIGCHLD信号处理函数
    避免僵尸进程，并记录后台进程结束
*/
void HandleSigChld(int sig) {
    int status;
    pid_t pid;
    
    // 使用WNOHANG非阻塞地回收所有已结束的子进程
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // 标记后台进程为已结束
        for (int i = 0; i < bg_count; i++) {
            if (bg_processes[i].pid == pid && bg_processes[i].active) {
                bg_processes[i].active = 0;  // 标记为已结束
                printf("\n[%d] + 完成\t%s\n", pid, bg_processes[i].command);
                printf("#  ");  // 重新显示提示符
                fflush(stdout);
                break;
            }
        }
    }
}

/*
    添加后台进程到列表
*/
void AddBgProcess(pid_t pid, char* command) {
    if (bg_count < MAX_BG_PROCESSES) {
        bg_processes[bg_count].pid = pid;
        strncpy(bg_processes[bg_count].command, command, 255);
        bg_processes[bg_count].command[255] = '\0';
        bg_processes[bg_count].active = 1;
        printf("[%d] %d\n", bg_count + 1, pid);  // 打印作业号和进程ID
        bg_count++;
    }
}

/*
    检查后台进程状态并清理
*/
void CheckBgProcesses() {
    int i = 0;
    while (i < bg_count) {
        if (!bg_processes[i].active) {
            // 移除已结束的进程
            for (int j = i; j < bg_count - 1; j++) {
                bg_processes[j] = bg_processes[j + 1];
            }
            bg_count--;
        } else {
            i++;
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
    // 处理空命令
    if (command == NULL || *command == '\0') {
        return NULL;
    }
    
    // 如果命令包含空格，需要特殊处理
    // 但这里我们只处理第一个参数（程序名）
    
    char* path = getenv("PATH");
    if (path == NULL) {
        return NULL;
    }
    
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
    修改：正确处理空格分隔的参数
*/

int ParseCommand(char* command, char* args[], int* isback) {
    int cnt = 0;
    int len = strlen(command);
    
    *isback = 0;
    
    // 去除开头和结尾的空格
    char* start = command;
    while (*start == ' ' || *start == '\t') start++;
    
    char* end = command + len - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n')) {
        *end = '\0';
        end--;
    }
    
    // 检查末尾是否有&
    if (end >= start && *end == '&') {
        *isback = 1;
        *end = '\0';  // 移除&
        
        // 再去除&之前的空格
        end--;
        while (end >= start && (*end == ' ' || *end == '\t')) {
            *end = '\0';
            end--;
        }
    }
    
    // 如果命令为空，返回
    if (*start == '\0') {
        args[0] = NULL;
        return 0;
    }
    
    // 使用空格和制表符作为分隔符
    char* token = strtok(start, " \t");
    
    while(cnt < MAX_ARGS - 1 && token != NULL) {
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

void FirstShow() {
    int time;
    char flag = '#';
    char* wl = "Welcome";
    char* wl2 = "Shell";
    
    printf("\n");  // 先空一行
    
    for(time = 1; time <= 10; time++) {
        for(int i = 0; i < 30; i++) {
            printf("%c", flag);
        }
        printf("\n");
        
        if(time == 5) {  // 10/2 = 5
            // 打印第一行 "Welcome"
            for(int i = 0; i < 10; i++) {
                printf("%c", flag);
            }
            printf("%s%s%s", COLOR_WELCOME, wl, COLOR_RESET);
            for(int i = 0; i < 13; i++) {
                printf("%c", flag);
            }
            printf("\n");
            
            // 打印第二行 "Shell"
            for(int i = 0; i < 11; i++) {
                printf("%c", flag);
            }
            printf("%s%s%s", COLOR_WELCOME, wl2, COLOR_RESET);
            for(int i = 0; i < 14; i++) {
                printf("%c", flag);
            }
            printf("\n");
        }
        
        usleep(50000);
    }
    
    printf("\n");  // 再空一行
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
    char answer[10];
    char* execpath = NULL;
    pid_t pidChild;
    int status, isback = 0;
    char current_path[MAX_PATH];  // 用于存储当前路径
    
    while(1) {
        CheckBgProcess();
        
        // 获取并显示当前工作目录
        if (getcwd(current_path, sizeof(current_path)) != NULL) {
            printf("%s# ", current_path);  // 显示当前路径
        } else {
            printf("# ");
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

        // 处理cd命令
        if (strncmp(command, "cd", 2) == 0 && (command[2] == ' ' || command[2] == '\0')) {
            ParseCommand(command, args, &isback);
            
            // 如果没有参数，cd到home目录
            if (args[1] == NULL || strcmp(args[1], "~") == 0) {
                char *home = getenv("HOME");
                if (home == NULL) {
                    fprintf(stderr, "cd: HOME环境变量未设置\n");
                } else if (chdir(home) != 0) {
                    perror("cd");
                }
            }
            // 处理"cd -"回到上一个目录
            else if (strcmp(args[1], "-") == 0) {
                static char prev_path[MAX_PATH] = "";
                if (prev_path[0] == '\0') {
                    fprintf(stderr, "cd: 没有上一个目录\n");
                } else {
                    char temp[MAX_PATH];
                    if (getcwd(temp, sizeof(temp)) != NULL) {
                        if (chdir(prev_path) == 0) {
                            strcpy(prev_path, temp);
                            printf("%s\n", prev_path);
                        } else {
                            perror("cd");
                        }
                    }
                }
            }
            // 普通目录切换
            else {
                char target_path[MAX_PATH];
                
                // 处理相对路径和绝对路径
                if (args[1][0] == '/') {
                    strncpy(target_path, args[1], MAX_PATH - 1);
                    target_path[MAX_PATH - 1] = '\0';
                } else {
                    // 保存上一个目录
                    static char prev_path[MAX_PATH] = "";
                    if (getcwd(target_path, sizeof(target_path)) != NULL) {
                        strcpy(prev_path, target_path);
                        strncat(target_path, "/", MAX_PATH - strlen(target_path) - 1);
                        strncat(target_path, args[1], MAX_PATH - strlen(target_path) - 1);
                    }
                }
                
                // 执行目录切换
                if (chdir(args[1]) != 0) {
                    perror("cd");
                }
            }
            continue;  // 继续下一次循环
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

                if(WIFEXITED(status) && WEXITSTATUS(status) == 1) {
                    Error(1);
                }
            }
            break;
        }
    }
}