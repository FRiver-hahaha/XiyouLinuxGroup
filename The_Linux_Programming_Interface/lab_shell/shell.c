// 3.1

#include <unistd.h>
#include <stdlib.h>

#define COLOR_RESET "\033[0m" // 重置 
#define COLOR_WELCOME "\033[1;34m" // 欢迎界面(粗体蓝色)

void FirstShow();
void Shell();

int main() {
    FirstShow();
    exit(EXIT_SUCCESS);
}

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
            for(int i = 0; i < 12; i++) {
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

void Shell() {
    char command[255];
    pid_t pidChild;
    int count = 0;
    write(1, "# ", 2);
    count = read(0, command, 255);
    command[count - 1] = '\0';
    switch (pidChild = fork())
    {
    case -1:
        perror("fork");
        break;
    
    case 0:
        execve(command, 0, 0);
        break;
    default:
        wait(NULL);
        break;
    }
}