
#include <fcntl.h>  /* Definition of AT_* constants */
#include <stdio.h>
#include <dirent.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <setjmp.h>
#include <sys/resource.h>

/* color */
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"
#define RED_BOLD "\x1b[;31;1m"
#define BLU_BOLD "\x1b[;34;1m"
#define YEL_BOLD "\x1b[;33;1m"
#define GRN_BOLD "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC "\x1b[;36;1;3m"
#define RESET "\x1b[0;m"

/* 全域變數，放解析過後的使用者指令（字串陣列）*/
char* argVect[256];

/* jumpbuffer，setjmp 和 longjmp 使用。類似於書籤，只能跳到「caller」*/
sigjmp_buf jumpBuf;

/* shell 是否產生 child process，決定 ctrl c 要送給誰 */
volatile sig_atomic_t hasChild = 0;

/* child 的 process id */ 
pid_t childPid;

/* 每秒鐘有多少個 nanoseconds */
const long long nspersec = 1000000000;

long long timespec2sec(struct timespec ts) {
    long long ns = ts.tv_nsec;
    ns += ts.tv_sec * nspersec;
    return ns;
    //return (double)ns/1000000000.0;
}

/* 將 timeval 資料結構轉換成多少個 nanoseconds */
double timeval2sec(struct timeval input) {
    long long us = input.tv_sec*1000000;
    us += input.tv_usec;
    //printf("%ldsec, %ld us\n", input.tv_sec, input.tv_usec);
    return (double)us/1000000.0;
}

/* signal handler 專門用來處理 ctrl c */
void ctrlC_handler(int sigNumber) {
    /* 如果 shell 有 child，結束掉 child。例如：正在執行 ls */
    if (hasChild) {
        kill(childPid, sigNumber);
        hasChild = 0;
    }
    /* 否則先將「^c」放到 input stream，然後讓主迴圈決定怎樣處理「^c」*/
    else {
        printf("\n");
        if (argVect[0] == NULL) {
            /* 推到 stdin 的 buffer 中 */
            ungetc('\n', stdin);
            ungetc('c', stdin);
            ungetc('^', stdin);
            longjmp(jumpBuf, 1);
        } else {
            /* 極少發生，剛好在處理字串，忽略這個 signal，印出訊息提示一下 */
            fprintf(stderr, "info, 處理字串時使用者按下 ctrl c\n");
        }
    }
}

/* dir 可以列出當前目錄裡的所有檔案，功能類似於 ls */
int dir() {
    DIR *dir;
    struct dirent *ent;
    char *curDir = "./";
    char pathname[512];
    struct stat buf;
    int perm;
    char *time;
    
    dir = opendir("./");
    ent = readdir(dir);
    while (ent != NULL) {
        strcpy(pathname, curDir);
        strcat(pathname, ent->d_name);
        stat(pathname, &buf);
        perm = (buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
        time = ctime(&buf.st_atime);
        time[strlen(time) - 1] = 0;
        printf("%o  %d  %d %8d %s %s\n", perm, buf.st_uid, buf.st_gid, (int)buf.st_size, time, ent->d_name);
        ent = readdir(dir);
    }
    closedir(dir);
    return 0;
}

void parseString(char* str, char** cmd) {
    int idx = 0;
    char* token;

    token = strtok(str, " \n");
    while(token != NULL) {
        argVect[idx++] = token;
        if (idx == 1) {
            *cmd = token;
        }
        token = strtok(NULL, " \n");
    }
    argVect[idx] = NULL; // string ending
}

int main (int argc, char** argv) {
    char cmdLine[4096];
    char hostName[256];
    char cwd[256];
    char *exeName;
    int pid, pos, wstatus;
    
    struct rusage resUsage; // 資源使用率，shell 可使用 wait3() 得到 child 的資源使用率
    struct timespec statTime, endTime; // 外部程式的開始執行時間、結束時間
    
    /* 註冊 signal 的處理方式 */
    signal(SIGINT, ctrlC_handler);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    
    /* main loop */
    while(1) {

        int homeLen = 0;
        char *showPath, *loginName = getlogin(); // 抓主機名稱、用戶名稱
        gethostname(hostName, 256);
        
        hasChild = 0;
        argVect[0]=NULL;

        /* 製造要顯示的路徑字串，會參考 "home" 將 "home" 路徑取代為 ~ */
        getcwd(cwd, 256);
        /* 目的是：把/home/shiwulo/Desktop 轉換成 ~/Desktop */
        pos=strspn(getenv("HOME"), cwd);
        homeLen = strlen(getenv("HOME"));
        //printf("cwd=%s, home=%s, pos=%d, prompt=%s", cwd, getenv("HOME"), pos, &cwd[pos]);
        if(pos>=homeLen) {
            cwd[pos-1]='~';
            showPath=&cwd[pos-1];
        }
        else
            showPath=cwd;
        
        /* 印出提示符號 */
        printf(LIGHT_GREEN"%s@%s:", loginName, hostName);
        printf(BLU_BOLD"%s>> " NONE, showPath);
        
        /* 設定返回地點，如果使用者按下 ctrl c 會從 setjmp 的下一行開始執行 */
        setjmp(jumpBuf);

        /* 接收使用者命令，除了 cd, exit, dir, ^c 以外，其他指令呼叫對應的執行檔 */
        fgets(cmdLine, 4096, stdin);

        if (strlen(cmdLine) > 1)  // 長度是否 >1，判斷「使用者無聊按下 enter 鍵」
            parseString(cmdLine, &exeName);
        else
            continue;

        /* 處理 ctrl c */
        if (strcmp(exeName, "^c") == 0) {   
            printf("\n");
            continue;
        }

        /* dir 其實就是 ls */
        if (strcmp(exeName, "dir")==0) {
            dir();
            printf("\n");
            continue;
        }

        /* 結束 shell，因此跳出 while(1)，exit 為內建指令 */
        if (strcmp(exeName, "exit") == 0)
            break;
        
        /* cd 為內建指令，改變 shell 的工作目錄且 shell 的 child 會繼承這個工作目錄 */
        if (strcmp(exeName, "cd") == 0) {
            if (strcmp(argVect[1], "~")==0)
                chdir(getenv("HOME"));
            else
                chdir(argVect[1]);
            continue;
        }

        /* 外部指令的部分 */
        clock_gettime(CLOCK_MONOTONIC, &statTime);
        pid = fork();
        if (pid == 0) {
            if (execvp(exeName, argVect) == -1) {
                perror("myShell");
                exit(errno*-1);
            }
        } else {
            childPid = pid; /* 通知 singal handler，如果使用者按下 ctrl c時，要處理這個 child */
            hasChild = 1; /* 通知 singal handler，正在處理 child */

            wait3(&wstatus, 0, &resUsage);
            clock_gettime(CLOCK_MONOTONIC, &endTime);

            double sysTime = timeval2sec(resUsage.ru_stime);
            double usrTime = timeval2sec(resUsage.ru_utime);
            printf("\n");

            // long long elapse = timespec2sec(endTime)-timespec2sec(statTime);
            // printf(RED"經過時間:                                "YELLOW"%lld.%llds\n",elapse/nspersec, elapse%nspersec);
            // printf(RED"CPU 花在執行程式的時間：                 "YELLOW"%fs\n"
            //        RED"CPU 於 usr mode 執行此程式所花的時間：   "YELLOW"%fs\n"
            //        RED"CPU 於 krl mode 執行此程式所花的時間：   "YELLOW"%fs\n", sysTime+usrTime , usrTime, sysTime);
            // printf(RED"Page fault，但沒有造成 I/O：             "YELLOW"%ld\n", resUsage.ru_minflt);
            // printf(RED"Page fault，並且觸發 I/O：               "YELLOW"%ld\n", resUsage.ru_majflt);
            // printf(RED"自願性的 context switch：                "YELLOW"%ld\n", resUsage.ru_nvcsw);
            // printf(RED"非自願性的 context switch：              "YELLOW"%ld\n", resUsage.ru_nivcsw);
            // printf(RED "return value of " YELLOW "%s" RED " is " YELLOW "%d\n", exeName, WEXITSTATUS(wstatus));

            if (WIFSIGNALED(wstatus))
                printf(RED"the child process was terminated by a signal "YELLOW"%d"RED", named " YELLOW "%s.\n",  WTERMSIG(wstatus), strsignal(WTERMSIG(wstatus)));
            printf(NONE);
        }
    }
}