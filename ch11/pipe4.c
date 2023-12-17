// usage: pipe4-2
// 結果相當於執行: ls | wc

#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    
    int pipefd[2];
    int ret, wstat, pid1, pid2;
    
    pipe(pipefd);
    pid1 = fork();
    
    if (pid1 == 0) {
        close(1);           // 關閉 stdout
        dup(pipefd[1]);     // 將 pipefd[1] 複製到 stdout
        close(pipefd[1]);   // 將沒用到的關閉
        close(pipefd[0]);   // 將沒用到的關閉
        execlp("ls", "ls", NULL);   // 執行ls，ls 會將東西藉由 stdout 輸出到 pipefd[1]
    } 
    else {
        printf("1st child's pid = %d\n", pid1);
    }
    if (pid1 > 0) {    
        pid2 = fork();
        if (pid2 == 0) {
            close(0);           // 關閉 stdin
            dup(pipefd[0]);     // 將 pipefd[0] 複製到 stdin
            close(pipefd[1]);   // 將沒用到的關閉
            close(pipefd[0]);   // 將沒用到的關閉
            execlp("wc", "wc", NULL);   // 執行 wc，wc 將透過 stdin 從 pipefd[0] 讀入資料
        } else printf("2nd child's pid = %d\n", pid2);
    }
    // parent 一定要記得關掉 pipe 不然 wc 不會結束（因為沒有接到EOF）
    close(pipefd[0]); close(pipefd[1]);
    printf("child %d\n",wait(&wstat));
    printf("child %d\n",wait(&wstat));
}