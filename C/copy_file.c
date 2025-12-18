/*这个程序实现了复制相对路径下的文件的操作，可处理名字冲突时候的情况
使用方法：在终端内输入：可执行文件名 目标读取文件名 目标复制文件名*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
	char* res;
	int len = strlen(argv[1]) - 4;
	char buf[7];
	strcpy(buf,"(1)");
	strcpy(buf + 3,argv[1] + len);
	if(strncmp(argv[1], argv[2], len) == 0) {
		res = (char*)malloc(sizeof(char) * (strlen(argv[2]) + 3));
		memmove(res, argv[2], len);
		memmove(res + len, buf, 7);
		res[strlen(argv[2]) + 3] = '\0';
	}
	FILE* fpsrc,* fpres;
	fpsrc = fopen(argv[1], "rb");
	if(!fpsrc) {
		printf("No Found File!");
		exit(0);
	}
	
	char ch;
	fpres = fopen(res, "wb");
	if(!fpres) {
		printf("No Found File!");
		exit(0);
	}
	
	while(!feof(fpsrc)) {
		ch = fgetc(fpsrc);
		fputc(ch, fpres);
	}
	fclose(fpsrc);
	fclose(fpres);
	free(res);
	
	return 0;
}
