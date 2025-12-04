#include<dirent.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<grp.h>
#include<pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define COLOR_RESET   "\033[0m"
#define COLOR_DIR     "\033[1;4;31m"  // 粗体蓝色
#define COLOR_EXE     "\033[1;32m"  // 粗体绿色
#define COLOR_LINK    "\033[1;36m"  // 粗体青色
#define COLOR_SOCKET  "\033[1;35m"  // 粗体紫色（socket）
#define COLOR_PIPE    "\033[33m"    // 黄色（管道）
#define COLOR_BLOCK   "\033[1;33m"  // 粗体黄色（块设备：u盘，固态)
#define COLOR_CHAR    "\033[1;33m"  // 粗体黄色（字符设备:键鼠）

void isfastoutput(int argc);
void listFiles(const char* dirpath);

int main(int argc, char* argv[]) {
    
    if(argc == 1) {
        listFiles(".");
        printf("\n");
    }else {
        listFiles(*(argv++ + 1));
        printf("\n");
    }

    return 0;
}

void isfastoutput(int argc);

void listFiles(const char* dirpath) {
    DIR* dir = opendir(dirpath);
    struct dirent* dp;
    struct stat st;
    char fullpath[1024];
    while(1) {
        dp = readdir(dir);
        if(!dp) {
            break;
        }
        
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp->d_name);
        lstat(fullpath, &st);

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

