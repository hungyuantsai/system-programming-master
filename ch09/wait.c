/*
usage: 

1st terminal
$ ./wait

2nd terminal
$ kill -STOP pid
$ kill -STOP pid
$ kill -CONT pid
$ kill -TERM pid
*/

#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    pid_t cpid, w;
    int wstatus;
    cpid = fork();
    if (cpid == 0) {            
        /* Code executed by child */
        // 把 pid 記下來，到第二個 terminal 發出 signal 給這個 pid
        printf("Child PID is %ld\n", (long) getpid());

        /* Wait for signals */
        pause();
    } else {                   
         /* Code executed by parent */
        do {
            // 等待 child 執行結束
            w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
            // 用一堆內建的 macro，檢查 child 的狀態
            if (WIFEXITED(wstatus))
                printf("Parent: child is exited, status=%d\n", WEXITSTATUS(wstatus));
            if (WIFSIGNALED(wstatus))
                printf("Parent: child is killed by signal %d\n", WTERMSIG(wstatus));
            if (WIFSTOPPED(wstatus))
                printf("Parent: child is stopped by signal %d\n", WSTOPSIG(wstatus));
            if (WIFCONTINUED(wstatus))
                printf("Parent: child is continued\n");
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

        /* 當子行程沒有結束並且沒有被signal終止 */
        printf("Parent: bye bye\n");
        exit(EXIT_SUCCESS);
    }
}
