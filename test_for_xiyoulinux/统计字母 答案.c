#include <stdio.h>
#include <string.h>
/*
    统计字母
    现有一个数组，里面有数量不等的字符，请你使用另一个数组，来记录各个字符在字符串内的数量，要求做到不修改main函数内的代码。
    考点：哈希表
*/

void your_function(int ar[], char string[]);
void show_ar(int ar[]);

int main() {
  char welcome[] = "welcome to xiyou linux group!";
  int ar[26] = {0};
  your_function(ar, welcome);
  show_ar(ar);

  return 0;
}

void your_function(int ar[], char string[]) {
  for (int i = 0; i < strlen(string); ++i) {
    char ch = string[i];
    if (ch >= 'a' && ch <= 'z') {
      ar[ch - 'a']++;
    }
  }
}

void show_ar(int ar[]) {
  for (int i = 0; i < 26; ++i) {
    printf("%c: %d\n", (char)(i + 97), ar[i]);
  }
}