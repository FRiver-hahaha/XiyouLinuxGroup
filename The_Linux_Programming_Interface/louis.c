#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>

// 掩码确定参数 
#define Ca              0b1               // 显示隐藏文件排列1
#define Cl              0b10              // 详细排列
#define CR              0b100             // 递归排列    
#define Ct              0b1000            // 按照最新一次修改时间降序排列1
#define Cr              0b10000           // 逆序排列1
#define Ci              0b100000          // 显示inode编号排列1
#define Cs              0b1000000         // 显示已用内存块数量排列1

// 确定颜色
#define COLOR_RESET     "\033[0m"
#define COLOR_DIR       "\033[1;34m"    // 粗体蓝色
#define COLOR_EXE       "\033[1;32m"    // 粗体绿色
#define COLOR_LINK      "\033[1;36m"    // 粗体青色
#define COLOR_SOCKET    "\033[1;35m"    // 粗体紫色（socket）
#define COLOR_PIPE      "\033[33m"      // 黄色（管道）
#define COLOR_BLOCK     "\033[1;33m"    // 粗体黄色（块设备：u盘，固态)
#define COLOR_CHAR      "\033[1;33m"    // 粗体黄色（字符设备:键鼠）

void listFiles(const char*, int, int);//根据参数，普通列出目录下的文件
void printWithis(int, struct stat*, int, struct dirent**);//显示
int isfastoutput(int, char*[]);//命令行中传入的参数是否只有该可执行文件
int whatCommand(int, char*[]);//确定参数
int CompareListNormal(const struct dirent**, const struct dirent**);//按照字符顺序排列
int CompareListTime(const struct dirent**, const struct dirent**);//按照最新一次修改时间排列
int HowManyDirpath(int, char*[]);//确定目标路径的数量
int shouldPrintA(int, struct dirent*);//是否需要打印隐藏文件
int maxFileLength(int, struct dirent**, int, int);//确定每个文件的宽度
char* getColor(struct stat);//返回颜色

int main(int argc, char* argv[]) {
    int tmpargc = HowManyDirpath(argc, argv);
    if(isfastoutput(argc,argv)) {
        int command = whatCommand(argc, argv);
        listFiles(".", command, tmpargc);
        printf("\n");
    }else {
        int command = whatCommand(argc, argv);
        for(int i = 1; i < argc; i++) {
            listFiles(argv[i], command, tmpargc);
            printf("\n");
        }
    }
    exit(EXIT_SUCCESS);
}

char* getColor(struct stat st) {
    char* color = COLOR_RESET;
    if(S_ISDIR(st.st_mode)) color = COLOR_DIR;
    else if(S_ISLNK(st.st_mode)) color = COLOR_LINK;
    else if(S_ISSOCK(st.st_mode)) color = COLOR_SOCKET;
    else if(S_ISFIFO(st.st_mode)) color = COLOR_PIPE;
    else if(S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode)) color = COLOR_BLOCK;
    else if(st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) color = COLOR_EXE;
    return color;
}

int CompareListTime(const struct dirent** a, const struct dirent** b) {
    struct stat sta, stb;
    lstat((*a)->d_name, &sta);lstat((*b)->d_name, &stb);
    return sta.st_mtime >= stb.st_mtime ? -1 : 1;
}

int isfastoutput(int argc, char* argv[]) {
    int cnt = 0;        
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-') cnt++;
    }
    if(cnt == argc - 1) return 1;
    return 0;
}

void listFiles(const char* dirpath, int command, int tmpargc) {
    if(dirpath[0] == '-') return;
    if(access(dirpath, F_OK) != 0) {
        fprintf(stderr, "无法访问 '%s': 没有那个文件或目录\n", dirpath);
        return;
    }
    int n, maxLength, maxName, Time, enter = 2, term_width = 80;//默认
    long long sum = 0;
    struct dirent** dp;
    struct stat st;
    struct winsize w;
    char fullpath[1024];
    char** Rarr  = (char**)malloc(sizeof(char*) * 1024 * 1024);
    if(command & Ct) n = scandir(dirpath, &dp, NULL, CompareListTime);
    else n = scandir(dirpath, &dp, NULL, CompareListNormal);
    if(n < 0 && dirpath[0]) {
        perror("scandir");
        return;
    }
    if(tmpargc > 1) printf("%s:\n",dirpath);
    if(command & Cs) {
        for(int i = 0; i < n; ++i) {
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp[i]->d_name);
            lstat(fullpath, &st);
            if(!shouldPrintA(command, dp[i])) continue;
            sum += st.st_blocks / 2;
        }
        printf("总计 %lld\n",sum);
    }
    if(!(command & Cl)) {//根据是否需要详细排列，分成两种方案
        if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)term_width = w.ws_col;
        maxLength = maxFileLength(command, dp, n, 0);
        maxName = maxFileLength(command, dp, n, 3);
        Time = term_width / (maxLength + 4);
        Time = Time <= 0 ? 1 : Time;
        for(int i = 0; i < n && !(command & Cr); i++) {
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp[i]->d_name);//将几个字符串以整体的形式送到缓冲区，且函数本身可以防止溢出
            lstat(fullpath, &st);//获取文件详细信息
            if(!shouldPrintA(command, dp[i])) continue;
            if(enter > Time && i != 0) printf("\n");
            char* color = COLOR_RESET;
            printWithis(command, &st, n, dp);
            if(enter <= Time) {
                printf("%s%*s%s  ", getColor(st), maxName, dp[i]->d_name, COLOR_RESET);
                enter++;
            }
            else {
                printf("%s%*s%s  ", getColor(st), maxName, dp[i]->d_name, COLOR_RESET);
                enter = 2;
            }
        }
        for(int i = n - 1; i >= 0 && (command & Cr); i--) {
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp[i]->d_name);//将几个字符串以整体的形式送到缓冲区，且函数本身可以防止溢出
            lstat(fullpath, &st);//获取文件详细信息
            if(!shouldPrintA(command, dp[i])) continue;
            if(enter > Time && i !=  n - 1) printf("\n");
            char* color = COLOR_RESET;
            printWithis(command, &st, n, dp);
            if(enter <= Time) {
                printf("%s%*s%s  ", getColor(st), maxName, dp[i]->d_name, COLOR_RESET);
                enter++;
            }
            else {
                printf("%s%*s%s  ", getColor(st), maxName, dp[i]->d_name, COLOR_RESET);
                enter = 2;
            }
        }
    }
    else {
        char tmbuffer[80];
        struct stat st;
        struct tm* tm;
        struct passwd* pw;
        struct group* gr;
        char str[11];
        for(int i = 0 ; i < n && !(command & Cr); i++) {
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp[i]->d_name);
            lstat(fullpath, &st);
            if(!shouldPrintA(command, dp[i])) continue;
            printWithis(command, &st, n, dp);
            str[0] = '?';
            if(S_ISDIR(st.st_mode)) str[0] = 'd';
            else if(S_ISLNK(st.st_mode)) str[0] = 'l';
            else if(S_ISSOCK(st.st_mode)) str[0] = 's';
            else if(S_ISFIFO(st.st_mode)) str[0] = 'p';
            else if(S_ISBLK(st.st_mode)) str[0] = 'b';
            else if(S_ISCHR(st.st_mode)) str[0] = 'c';
            else str[0] = '-';
            str[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
            str[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
            str[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
            str[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
            str[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
            str[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
            str[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
            str[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
            str[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';
            str[10] = '\0';
            tm = localtime(&st.st_mtime);
            strftime(tmbuffer, sizeof(fullpath), "%m月 %H:%M", tm);
            pw = getpwuid(st.st_uid);
            gr = getgrgid(st.st_gid);
            printf("%s %lu %s %s %ld %s %s%s%s\n", str, st.st_nlink, pw->pw_name, gr->gr_name, st.st_size, tmbuffer, getColor(st), dp[i]->d_name, COLOR_RESET);
        }

        for(int i = n - 1 ; i >= 0 && (command & Cr); i--) {
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp[i]->d_name);
            lstat(fullpath, &st);
            if(!shouldPrintA(command, dp[i])) continue;
            printWithis(command, &st, n, dp);
            str[0] = '?';
            if(S_ISDIR(st.st_mode)) str[0] = 'd';
            else if(S_ISLNK(st.st_mode)) str[0] = 'l';
            else if(S_ISSOCK(st.st_mode)) str[0] = 's';
            else if(S_ISFIFO(st.st_mode)) str[0] = 'p';
            else if(S_ISBLK(st.st_mode)) str[0] = 'b';
            else if(S_ISCHR(st.st_mode)) str[0] = 'c';
            else str[0] = '-';
            str[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
            str[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
            str[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
            str[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
            str[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
            str[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
            str[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
            str[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
            str[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';
            str[10] = '\0';
            tm = localtime(&st.st_mtime);
            strftime(tmbuffer, sizeof(fullpath), "%m月 %H:%M", tm);
            pw = getpwuid(st.st_uid);
            gr = getgrgid(st.st_gid);
            printf("%s %lu %s %s %ld %s %s%s%s\n", str, st.st_nlink, pw->pw_name, gr->gr_name, st.st_size, tmbuffer, getColor(st), dp[i]->d_name, COLOR_RESET);
        }
    }
    if(command & CR) {
        int count = 0;
        for(int i = 0; i < n && !(command & Cr); i++) {
            if(strcmp(dp[i]->d_name, ".") == 0 || strcmp(dp[i]->d_name, "..") == 0) continue;
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp[i]->d_name);
            lstat(fullpath, &st);
            if(S_ISDIR(st.st_mode) && shouldPrintA(command, dp[i])) {
                Rarr[count] = (char*)malloc(sizeof(char) * (sizeof(fullpath) + 1));
                strcpy(Rarr[count], fullpath);
                count++;
            }
        }

        for(int i = n - 1; i >= 0 && command & Cr; i--) {
            if(strcmp(dp[i]->d_name, ".") == 0 || strcmp(dp[i]->d_name, "..") == 0) continue;
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, dp[i]->d_name);
            lstat(fullpath, &st);
            if(S_ISDIR(st.st_mode) && shouldPrintA(command, dp[i])) {
                Rarr[count] = (char*)malloc(sizeof(char) * (sizeof(fullpath) + 1));
                strcpy(Rarr[count], fullpath);
                count++;
            }
        }

        for(int i = 0; i < count; i++) {
            printf("\n%s:\n", Rarr[i]);
            listFiles(Rarr[i], command, tmpargc);
        }
        for(int i = 0; i < count; i++) free(Rarr[i]);
        free(Rarr);
    }
    for(int i = 0; i < n; i++) free(dp[i]);
    free(dp);
}

int whatCommand(int argc, char* argv[]) {
    int command = 0;
    if(argc == 1) return 0;
    for(int i = 1; i < argc; ++i) {
        if(argv[i][0] != '-') continue;
        else {
            int enter = 1;
            while(argv[i][enter] != '\0') {
                switch (argv[i][enter++]) {
                    case 'a':command |= Ca;break;
                    case 'l':command |= Cl;break;
                    case 'R':command |= CR;break;
                    case 't':command |= Ct;break;
                    case 'r':command |= Cr;break;
                    case 'i':command |= Ci;break;
                    case 's':command |= Cs;break;
                    default:fprintf(stderr, "可用选项：-a, -l, -R, -t, -r, -i, -s\n");
                       exit(EXIT_FAILURE);
                }
            }
        }
    }
    return command;
}

int CompareListNormal(const struct dirent** a, const struct dirent** b) {
    return strcmp((*a)->d_name, (*b)->d_name);
}

void printWithis(int command, struct stat* st, int n, struct dirent** dpall) {
    if(command & Ci) {
        int mask = 1;
        long int tmp = st->st_ino;
        do {
            tmp /= 10;
            mask++;
        }while(tmp > 9);
        int length = maxFileLength(command, dpall, n, 1);

        printf("%*lu ", length, st->st_ino);
    }
    if(command & Cs) {
        int mask = 1;
        long int tmp = st->st_blocks / 2;
        do {
            tmp /= 10;
            mask++;
        }while(tmp > 9);
        int length = maxFileLength(command, dpall, n, 2);

        printf("%*ld ", length, st->st_blocks / 2);
    }
}

int HowManyDirpath(int argc, char* argv[]) {
    int cnt = 0;
    for(int i = 1; i < argc; i++) if(argv[i][0] != '-') cnt++;
    return cnt;
}

int shouldPrintA(int command, struct dirent* dp) {
    if(dp->d_name[0] == '.' || strcmp(dp->d_name, "..") == 0) return (command & Ca);
    return 1;
}

int maxFileLength(int command, struct dirent** dp, int n, int res) {
    int max_name = 0, max_i = 0, max_s = 0, r = 0;
    struct stat st;
    for(int i = 0; i < n; ++i) {
        if(!shouldPrintA(command, dp[i])) continue;
        max_name = ((int)strlen(dp[i]->d_name) >= max_name ) ? (int)strlen(dp[i]->d_name) : max_name;
    }
    for(int i = 0; i < n && command & Ci; ++i) {
        if(!shouldPrintA(command, dp[i])) continue;
        int mask = 1;
        int tmp = dp[i]->d_ino;
        do {
            tmp /= 10;
            mask++;
        }while(tmp > 9);
        max_i = mask >= max_i ? mask : max_i;
    }
    for(int i = 0; i < n && command & Cs; ++i) {
        if(!shouldPrintA(command, dp[i])) continue;
        lstat(dp[i]->d_name, &st);
        int mask = 1;
        int tmp = st.st_blocks;
        do {
            tmp /= 10;
            mask++;
        }while(tmp > 9);
        max_s = mask >= max_s ? mask : max_s;
    }
    switch (res) {
        case 0:r = max_name + max_i + max_s;break;
        case 1:r = max_i;break;
        case 2:r = max_s;break;
        case 3:r = max_name;break;
    }
    return r; 
}