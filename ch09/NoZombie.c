/*
usage: ./NoZombie 10000
*/
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void manyChild(int num) {
    int i, pid;
    for (int i=0; i<num; i++) {
        pid = vfork();
        // child 立即結束
        if (pid == 0) exit(0);
        // 如果是 parent 的話，就繼續執行 for loop
        if (pid != 0) continue;
    }
}

int main(int argc, char** argv) {
    int pid, num;
    sscanf(argv[1], "%d", &num);
    pid = fork();
    // 首先 child 會執行 manyChild，child 產生了「孫子」以後，child 就會死掉
    // 然後 child 死掉以後，「孫子就沒有父親」，UNIX 會自動將這些沒有父親的 process 轉到 init 或者 systemd 下
    // 並且 init 或 systemd 都會不斷的執行 wait，因此可以確保孫子不會變成殭屍
    if (pid == 0) {
        manyChild(num);
        exit(0);
    }
    getchar();
}