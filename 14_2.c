#include<stdio.h>
#include<stdlib.h>
#define CSIZE 4

typedef struct _name {
    char* first_name;
    char* last_name;
}Name;

typedef struct _student {
    Name name;
    double grade[3];
    double average;
}Stu;

void getGrade(Stu Student[]);
void average(Stu Student[]);
void print(Stu Student[]);
void printAverage(Stu Student[]);

int main() {
    Stu student[CSIZE];
    int i;
    for(i = 0; i < CSIZE; i++) {
        printf("input your first_name:");
        scanf("%s",student[i].name.first_name);
        printf("\n");
        while(getchar() != '\n') {
            printf("input your last_name:");
            scanf("%s",student[i].name.last_name);
            printf("\n");
        }
    }

    getGrade(student);
    average(student);
    print(student);

    return 0;
}

void getGrade(Stu Student[]) {
    int i;
    for(i = 0; i < CSIZE; i++) {
        printf("input the %s's grades.",&Student[i].name.first_name);
        scanf("%lf %lf %lf",&Student[i].grade[0],&Student[i].grade[1],&Student[i].grade[2]);
    }
}

void average(Stu Student[]) {
    int i;
    for(i = 0; i < CSIZE; i++) {
        Student[i].average = (Student[i].grade[0] + Student[i].grade[1] + Student[i].grade[2])/3.0;
    }
}

void print(Stu Student[]) {
    int i;
    for(i = 0; i < CSIZE; i++) {
        printf("%s %s\n",Student[i].name.first_name,Student[i].name.last_name);
        printf("%lf %lf %lf\n",Student[i].grade[0],Student[i].grade[1],Student[i].grade[2]);
        printf("%lf\n",Student[i].average);
    }
}

void printAverage(Stu Student[]) {
    int i;

    for(i = 0; i < CSIZE; i++) {
        printf("%lf",Student[i].average);
        if(!(i == CSIZE - 1)) {
            printf("\n");
        }
    }
}