#include <iostream>
#include <unistd.h>
using namespace std;

int main() {
    for(int i = 0;i < 10; ++i) {
        sleep(1);
        printf("i = %d\n", i);
    }

    return 0;
}
