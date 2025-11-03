# include<stdio.h>

int add(int a,int b);
int mutiply(int a,int b);
void Callback(int (*pFun)(int,int));

int main() {
    int input = 0;
    scanf("%d",&input);
    switch(input) {
        case 1:Callback(add);break;
        case 2:Callback(mutiply);break;
    }

    return 0;
}

void Callback(int (*pFun)(int,int)) {
    int x = 0;
    int y = 0;
    int ret = 0;
    scanf("%d %d",&x,&y);
    ret = pFun(x,y);
    printf("%d",ret);
    return;
}

int add(int a,int b) {
    return a + b;
}

int mutiply(int a,int b) {
    return a * b;
}
