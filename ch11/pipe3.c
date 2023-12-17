#include <assert.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
	
	char buf[200];
	
	int ret;
    int pipefd[2];
	
    pipe(pipefd);
	
    ret = fork();
	assert(ret>=0);

	if (ret==0) {	        /* child */
		close(1);			/* 關閉 stdout */
		dup(pipefd[1]);		/* 將 pipefd 複製到 stdout */
		close(pipefd[1]);
		close(pipefd[0]);
		printf("hello");	/* 印出 “hello” 到 stdout */
	} else {
		close(0);			/* 關閉 stdin */
		dup(pipefd[0]);		/* 將 pipefd 複製到 stdin */
		close(pipefd[0]);
		close(pipefd[1]);
		scanf("%s", buf);	/* 從 stdin 讀入資料*/
		printf("parent: %s\n", buf);
	}
	return 0;
}

