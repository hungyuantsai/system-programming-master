#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

int main() {
	int fd;
	const int G = 1000000000;
	const int M = 1000000;

	printf("這個程式會在當前的目錄下，製造檔案myHole\n");
	fd = open("./myHole", O_RDWR| O_CREAT | O_TRUNC, S_IRUSR| S_IWUSR);
	if (fd <0)
		perror("無法製造myHole");
	
    // lseek 將『檔案指標』由開始位置向後移動 100G，lseek 比較可能出錯，用 assert 檢查一下
    assert(lseek(fd, 10*M, SEEK_SET) != -1);
	write(fd, "1", sizeof("1"));
	
    // lseek 將『檔案指標』由『目前』位置向後移動 100G，lseek 比較可能出錯，用 assert 檢查一下
	assert(lseek(fd, 10, SEEK_CUR) != -1);
    write(fd, "2", sizeof("2"));
	
    assert(lseek(fd, 10, SEEK_CUR) != -1);
    write(fd, "3\n", sizeof("3\n"));
	
    close(fd);
	system("ls myHole -alhs");
	return 0;
}
