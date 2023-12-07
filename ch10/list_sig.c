#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

void sighandler(int signumber) {
    printf("get a signal named '%d', '%s'\n", signumber, strsignal(signumber));
}

/*
假設系統中最多有 100 個 signal，每個號碼都拿去註冊 signalhandler
要特別注意一下，有些跟 process control 的signal 是無法註冊的，例如：SIGKILL 
*/

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
    for (idx = 0; idx < 100; idx++) {
        if (sig_exist[idx] == 1)
            printf("%2d %s\n", idx, strsignal(idx));
    }
    printf("my pid is %d\n", getpid());
    printf("press any key to resume\n");
    getchar();
    return 0;
}


