#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int j;
    char **ep;
    
    clearenv();
    for(j = 1; j < argc; j++) {
        putenv(argv[j]);
    }

    setenv("GREET", "hello world", 1);  
    unsetenv("BYE");

    for(ep = argv; *ep != NULL; ep++) {
        puts(*ep);
    }

    exit(EXIT_SUCCESS);
}