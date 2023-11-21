#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sched.h>

// 將timespec轉成nanoseconds
long ts_to_long(struct timespec t) {
	return t.tv_sec * 1000000000 + t.tv_nsec;
}

// 使用 clock gettime 用靜態連結會比較慢，因為不能用 vsdo，動態連結才能用
int main(int argc, char **argv) {
	int i = 0;
	
    // 接收 clock_gettime 的精准度
	struct timespec t_res;
	
    // 開始及結束時間
	struct timespec tt1, tt2;

	printf("不同參數下，clock_gettime的精準度\n");
	clock_getres(CLOCK_REALTIME, &t_res);
	printf("CLOCK_REALTIME resolution:\t\t %ld nanoseconds\n", t_res.tv_nsec);
	clock_getres(CLOCK_MONOTONIC, &t_res);
	printf("CLOCK_MONOTONIC resolution:\t\t %ld nanoseconds\n", t_res.tv_nsec);
	clock_getres(CLOCK_PROCESS_CPUTIME_ID, &t_res);
	printf("CLOCK_PROCESS_CPUTIME_ID resolution:\t %ld nanoseconds\n", t_res.tv_nsec);

	printf("印出執行『i++』花多少時間\n");
	clock_gettime(CLOCK_MONOTONIC, &tt1);
	i++;
	clock_gettime(CLOCK_MONOTONIC, &tt2);
	printf("i++ consumes %ld nanoseconds!\n", ts_to_long(tt2) - ts_to_long(tt1));
}
