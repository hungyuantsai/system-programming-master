#include <assert.h>
#include <signal.h>
#include <stdio.h>

/* 與rec_sig.c搭配使用，可以檢測送出的signal對方是否都收得到 */

int main() {
    int pid, signum, times;
    int i;
    
    printf("要將signal送給？process ID\n");
    scanf("%d", &pid);
    
    printf("要送出第幾號signal？signal number\n");
    scanf("%d", &signum);
    
    printf("要送出多少個signal？stimes\n");
    scanf("%d", &times);
    
    for (i = 0; i < times; i++) {
        assert(kill(pid, signum) == 0);
    }
}
