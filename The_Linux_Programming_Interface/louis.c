//12.7下一步该处理i,s参数，目前问题：加上参数不显示

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>

// 掩码确定参数 
#define Ca              1               // 显示隐藏文件排列
#define Cl              10              // 详细排列
#define CR              100             // 递归排列    
#define Ct              1000            // 按照最新一次修改时间降序排列
#define Cr              10000           // 逆序排列
#define Ci              100000          // 显示inode编号排列
#define Cs              1000000         // 显示已用内存块数量排列

// 确定颜色
#define COLOR_RESET      "\033[0m"
#define COLOR_DIR       "\033[1;34m"    // 粗体蓝色
#define COLOR_EXE       "\033[1;32m"    // 粗体绿色
#define COLOR_LINK      "\033[1;36m"    // 粗体青色
#define COLOR_SOCKET    "\033[1;35m"    // 粗体紫色（socket）
#define COLOR_PIPE      "\033[33m"      // 黄色（管道）
#define COLOR_BLOCK     "\033[1;33m"    // 粗体黄色（块设备：u盘，固态)
#define COLOR_CHAR      "\033[1;33m"    // 粗体黄色（字符设备:键鼠）

int isfastoutput(int);//命令行中传入的参数是否只有该可执行文件
char whatCommad(int, char*[]);//确定参数
void listFiles(const char*, int);//根据参数，普通列出目录下的文件
void LongList(const char*, struct dirent**, int, int);//详细列出目录下的文件
int CompareList(const struct dirent** a, const struct dirent** b);//按照字符顺序排列


int main(int argc, char* argv[]) {
    
    if(isfastoutput(argc)) {
        listFiles(".", 0);
    }else {
        int command = whatCommad(argc, argv);
        listFiles(*(argv++ + 1), command);
    }
    printf("\n");
    exit(EXIT_SUCCESS);
}



int isfastoutput(int argc) {
    if(argc == 1) {
        return 1;
    }
    return 0;
}

void listFiles(const char* dirpath, int command) {
    int n;
    struct dirent** dp;
    struct stat st;
    char fullpath[1024];

    if(!(command & Cl)) {//根据是否需要详细排列，分成两种方案
        n = scandir(dirpath, &dp, NULL, CompareList);
        for(int i = 0; i < n; i++) {
            
            
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp[i]->d_name);//将几个字符串以整体的形式送到缓冲区，且函数本身可以防止溢出
            lstat(fullpath, &st);//获取文件详细信息

            if(S_ISDIR(st.st_mode)) {
                if(command & Ci) 
                    printf("%-7lu ",st.st_ino);
                if(command & Cs) 
                    printf("%-7ld",st.st_blocks);
                printf(COLOR_DIR "%s" COLOR_RESET "\t", dp[i]->d_name);
            } else if(command & Ca && S_ISLNK(st.st_mode)) {
                printf(COLOR_LINK "%s" COLOR_RESET "\t", dp[i]->d_name);
            } else if(command & Ca && S_ISSOCK(st.st_mode)) {
                printf(COLOR_SOCKET "%s" COLOR_RESET "\t", dp[i]->d_name);
            } else if(command & Ca && S_ISFIFO(st.st_mode)) {
                printf(COLOR_PIPE "%s" COLOR_RESET "\t", dp[i]->d_name);
            } else if(command & Ca && S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode)) {
                printf(COLOR_BLOCK "%s" COLOR_RESET "\t", dp[i]->d_name);
            } else if(command & Ca && st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                // 可执行文件
                printf(COLOR_EXE "%s" COLOR_RESET "\t", dp[i]->d_name);
            } else {
                // 普通文件
                printf("%s\t", dp[i]->d_name);
            }

            
        }

        for(int i = 0; i < n; i++) {
            free(dp[i]);
        }
        free(dp);
    }
    else if(command & Cl) {
    }
    
    

}

char whatCommad(int argc, char* argv[]) {
    int command = 0;
    for(int i = 2; i < argc; ++i) {
        if(argv[i][0] != '-') {
            continue;
        }else {
            int j = 1;
            while(argv[i][j++] != '\0') {
                switch (argv[i][j]) {
                    case 'a':
                        command |= Ca;
                        break;
                    case 'l':
                        command |= Cl;
                        break;
                    case 'R':
                        command |= CR;
                        break;
                    case 't':
                        command |= Ct;
                        break;
                    case 'r':
                        command |= Cr;
                        break;
                    case 'i':
                        command |= Ci;
                        break;
                    case 's':
                        command |= Cs;
                        break;
                    default:
                        printf("Wrong Argument");
                        break;
                }
            }
        }
    }
}

void LongList(const char* dirpath, struct dirent** list, int n, int command) {
    char fullbuffer[1024];
    char tmbuffer[80];
    struct tm* tm;
    struct stat st;
}

int CompareList(const struct dirent** a, const struct dirent** b) {
    return strcmp((*a)->d_name, (*b)->d_name);
}
