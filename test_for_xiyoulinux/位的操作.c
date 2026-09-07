#include <stdio.h>

/*
    位的操作
    下面是一个int类型的数组，通过对每个数字进行位运算，可以生成一个新的结果，请你对当前位运算的代码进行解释。
*/

int main() {
  int nums[] = {167, 150, 134, 144, 138, 179, 150, 145,
                138, 135, 184, 141, 144, 138, 143};

  int size = sizeof(nums) / sizeof(nums[0]);
  for (int i = 0; i < size; i++) {
    int mask = 0;
    int bitval = 1;
    int bits = 8;
    while (bits-- > 0) {
      mask |= bitval;
      bitval <<= 1;
    }
    printf("%c", nums[i] ^ mask);
    if (i == size - 1) {
      printf("\n");
    }
  }

  return 0;
}