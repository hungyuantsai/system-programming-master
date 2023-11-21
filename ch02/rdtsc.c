#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sched.h>
#include <assert.h>

/*
rdtsc 的問題：
1. 同一處理器的多個核心之間，以及不同處理器的不同核心之間的 rdtsc 不確定會不會同步
2. 亂序執行可能導致程式讀的 tsc 不準確（使用 rdtscp 或是 memory barrier）
*/

extern __inline__ uint64_t rdtsc(void) {
    uint64_t msr;
    __asm__ __volatile__ (
        "rdtsc\n\t"
        "shl $32, %%rdx\n\t"
        "or %%rdx, %%rax\n\t"
	    "mov %%rax, %0"
        : "=m"(msr)
     	:
	    : "rax", "rdx"
    );
    return msr;
}

long ts_to_long(struct timespec t) {
	return t.tv_sec * 1000000000 + t.tv_nsec;
}

int main(int argc, char **argv) {
    int tmp = 0;
    uint64_t cycles1, cycles2;
    struct timespec ts1, ts2;

    printf("這個程式是量測一個指令執行的時間，但CPU可同時執行數十個指令\n");
    printf("因此這些量測方法比較適合量測大範圍的程式碼\n\n");

    cycles1 = rdtsc();
    tmp++;
    cycles2 = rdtsc();

    clock_gettime(CLOCK_MONOTONIC, &ts1);
    tmp++;
    clock_gettime(CLOCK_MONOTONIC, &ts2);

    printf("開始 %lu, 結束 %lu\n", cycles1, cycles2);
    printf("rdtscp: tmp++ consumes %lu cycles!\n\n", cycles2-cycles1);

    printf("開始 %lu, 結束 %lu\n",ts_to_long(ts1), ts_to_long(ts2));
    printf("clock_gettime: tmp++ consumes %lu nanoseconds!\n\n", ts_to_long(ts2)-ts_to_long(ts1));

    assert(system("cat /proc/cpuinfo | grep 'cpu MHz' | head -1")>=0);
}