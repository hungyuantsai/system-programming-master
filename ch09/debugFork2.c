/*
usage: debugFork2
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    pid_t pid;
    int waiting = 1;

    if ((pid = fork()) == 0) {
        // 在 child 的地方加入一個空迴圈，gdb attach 成功以後再從 gdb 中，將 waiting 設定為 0
        while (waiting)
            ;
        printf("child");
    } else {
        printf("child's pid = %d\n", pid);
        printf("parent");
    }
    printf("end\n");
    return 0;
}
