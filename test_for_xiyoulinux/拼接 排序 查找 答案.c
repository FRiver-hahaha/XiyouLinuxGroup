#include <stdio.h>
#include <stdlib.h>

/*
    拼接 排序 查找
    请你尽可能在不修改当前main函数内代码的情况下，实现两个数组的拼接，升序排序，查找
    考点：排序、二分查找
*/

void your_concat(int ar1[], int len1, int ar2[], int len2, int* ar3);
void your_sort(int* ar3, int len3);
int your_check(int* ar3, int len3, int check_number);
void show_ar3(int* ar3, int len3);

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

void your_concat(int ar1[], int len1, int ar2[], int len2, int* ar3) {
  for (int i = 0; i < len1; ++i) {
    *ar3 = ar1[i];
    ar3++;
  }
  for (int i = 0; i < len2; ++i) {
    *ar3 = ar2[i];
    ar3++;
  }
}

void your_sort(int* ar3, int len3) {
  for (int i = 0; i < len3 - 1; ++i) {
    for (int j = 0; j < len3 - i - 1; ++j) {
      if (ar3[j] > ar3[j + 1]) {
        int temp = ar3[j];
        ar3[j] = ar3[j + 1];
        ar3[j + 1] = temp;
      }
    }
  }
}

int your_check(int* ar3, int len3, int check_number) {
  int left = 0;
  int right = len3 - 1;

  while (left <= right) {
    int mid = left + (right - left) / 2;

    if (ar3[mid] == check_number) {
      return check_number;
    } else if (ar3[mid] < check_number) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }

  return -1;
}

void show_ar3(int* ar3, int len3) {
  for (int i = 0; i < len3; ++i) {
    printf("%d", ar3[i]);
    if (!(i == len3 - 1)) {
      printf(" ");
    }
  }
  printf("\n");
}