//12.7该处理参数，并且弄懂读取目录的函数

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
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
void listFiles(const char*, int);//根据参数，列出目录下的文件

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
    
    DIR* dir = opendir(dirpath);
    struct dirent* dp;
    struct stat st;
    char fullpath[1024];

    if(!command) {
        while(1) {
            dp = readdir(dir);
            if(!dp) {
                break;
            }
            
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp->d_name);//将几个字符串以整体的形式送到缓冲区，且函数本身可以防止溢出
            lstat(fullpath, &st);//获取文件详细信息
            

            if(S_ISDIR(st.st_mode)) {
                printf(COLOR_DIR "%s" COLOR_RESET "\t", dp->d_name);
            } else if(S_ISLNK(st.st_mode)) {
                printf(COLOR_LINK "%s" COLOR_RESET "\t", dp->d_name);
            } else if(S_ISSOCK(st.st_mode)) {
                printf(COLOR_SOCKET "%s" COLOR_RESET "\t", dp->d_name);
            } else if(S_ISFIFO(st.st_mode)) {
                printf(COLOR_PIPE "%s" COLOR_RESET "\t", dp->d_name);
            } else if(S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode)) {
                printf(COLOR_BLOCK "%s" COLOR_RESET "\t", dp->d_name);
            } else if(st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                // 可执行文件
                printf(COLOR_EXE "%s" COLOR_RESET "\t", dp->d_name);
            } else {
                // 普通文件
                printf("%s\t", dp->d_name);
            }
        }
    }

    while(1) {

        dp = readdir(dir);
        if(!dp) {
            break;
        }
        
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp->d_name);
        lstat(fullpath, &st);
        

        if(S_ISDIR(st.st_mode)) {
            printf("%ju "COLOR_DIR "%s" COLOR_RESET "\t", dp->d_ino, dp->d_name);
        } else if(S_ISLNK(st.st_mode)) {
            printf("%ju "COLOR_LINK "%s" COLOR_RESET "\t", dp->d_ino, dp->d_name);
        } else if(S_ISSOCK(st.st_mode)) {
            printf("%ju "COLOR_SOCKET "%s" COLOR_RESET "\t", dp->d_ino, dp->d_name);
        } else if(S_ISFIFO(st.st_mode)) {
            printf("%ju "COLOR_PIPE "%s" COLOR_RESET "\t", dp->d_ino, dp->d_name);
        } else if(S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode)) {
            printf("%ju "COLOR_BLOCK "%s" COLOR_RESET "\t", dp->d_ino, dp->d_name);
        } else if(st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
            // 可执行文件
            printf("%ju "COLOR_EXE "%s" COLOR_RESET "\t", dp->d_ino, dp->d_name);
        } else {
            // 普通文件
            printf("%ju ""%s\t",dp->d_ino , dp->d_name);
        }
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
                        print("Wrong Argument");
                        break;
                }
            }
        }
    }
}