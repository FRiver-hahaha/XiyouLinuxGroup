#include <stdio.h>
#include <stdlib.h>

/*
    拼接 排序 查找
    请你尽可能在不修改当前main函数内代码的情况下，实现两个数组的拼接，升序排序，查找
*/

int main() {
  int ar1[] = {1, 3, 5, 7, 9};
  int len1 = sizeof(ar1) / sizeof(ar1[0]);
  int ar2[] = {2, 4, 6, 8, 10, 12, 14};
  int len2 = sizeof(ar2) / sizeof(ar2[0]);
  int* ar3 = (int*)malloc(sizeof(int) * (len1 + len2));
  int len3 = len1 + len2;

  your_concat(ar1, len1, ar2, len2, ar3);
  show_ar3(ar3, len3);
  your_sort(ar3, len3);
  show_ar3(ar3, len3);

  int check_number = 3;
  int result_number = your_check(ar3, len3, check_number);

  if (result_number == check_number) {
    printf("Welcome to Xiyou Linux Group!\n");
    show_ar3(ar3, len3);
  } else {
    printf("failed...\n");
  }
  free(ar3);
  return 0;
}