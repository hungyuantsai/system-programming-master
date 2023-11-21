#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

int main() {
    char* p1;
    char* p2;
	char command[1024];
    // 印出行程的pid，方便我們到 /proc 目錄裡面找到對應的檔案 
    printf("pid = %d\n\n", getpid());

    // 配置 64byte 記憶體
    printf("malloc(64)\n");
	p1 = (char*)malloc(64);
	printf("p1=%p\n", p1);

	// 配置 256Kbyte 記憶體
	p2 = (char*)malloc(64*4096);
	printf("p2=%p\n\n", p2);

	sprintf(command, "pmap %d", getpid());
	assert(system(command) >=0);
}



