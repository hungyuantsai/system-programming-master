#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

/* 和 send_sig 搭配使用，程式中假設作業系統最多有 100 個 signal，每一個 signal 都去攔截看看 */

int nSig[100];

void sighandler(int signumber) {
    // 統計某一種 signal 收集了多少個
    nSig[signumber]++;
}

int main(int argc, char **argv) {
    int sig_exist[100];
    int idx = 0;
    for (idx = 0; idx < 100; idx++) {
        if (signal(idx, sighandler) == SIG_ERR) {
            sig_exist[idx] = 0;
        } else {
            sig_exist[idx] = 1;
        }
    }
    printf("my pid is %d\n", getpid());
    printf("現在這一個process已經可以開始接收signal了\n");
    printf("press any key to count the signal number\n");
    getchar();

    for (idx=0; idx<100; idx++) {
        if (nSig[idx] != 0)
            printf("signal #%d, %d times\n", idx, nSig[idx]);
    }
    
    return 0;
}


