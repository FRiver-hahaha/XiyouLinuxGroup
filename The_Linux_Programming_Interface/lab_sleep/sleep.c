#include <stdio.h>//fprintf()
#include <unistd.h>//sleep()
#include <stdlib.h>//exit()

int Howtime(char* string);

int main(int argc, char* argv[]) { 
  int total_time = 0;
  if(argc == 1) {
    fprintf(stderr, "sleep: argument...\n");
    exit(1);
  }
  
  for(int i = 1; i < argc; i++) {
    int tmp = Howtime(argv[i]);
    if(tmp < 0) {
      fprintf(stderr, "sleep:invalid time '%s'\n",argv[i]);
    }else {
      total_time += tmp;
    }
  }
  sleep(total_time);
  
  exit(0);
}

int Howtime(char* string) {
  if(string[0] == '-' || string[0] == '\0') {
    return -1;
  }
  int time = 0;
  int i = 0;
  while(string[i] != '\0') {
    if(string[i] < '0' && string[i] > '9') {
      return -1;
    }
    time = time * 10 + (string[i] - '0');
    i++;
  }
  return time;
}
