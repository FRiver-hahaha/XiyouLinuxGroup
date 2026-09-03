#include <stdio.h>

/*
    跳不出去的while循环
    你觉得这个循环可以结束吗？如果有，条件是什么；如果没有，请解释为什么。并且请你解释，a,b,c的变化情况
    考点：数据类型、自增自减、unsigned修饰
*/

int main() {
  int a;
  int b = 0;
  unsigned short c = 1;
  while (c >= 0) {
    a = b++;
    a--;
    b = ++a;
    --c;
  }
  return 0;
}