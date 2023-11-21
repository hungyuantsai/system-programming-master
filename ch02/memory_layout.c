#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

int global_a;
int global_b = 10;

int main() {
	int local_c = 10;
	int local_d;
	int *malloc_e = malloc(100 * sizeof(int));
	
    printf("印出指標的位址，因為 ASLR 的關係，每次印出來的結果會不一樣\n");
	printf(" global_a\t%p\n global_b\t%p\n local_c\t%p\n local_d\t%p\n malloc_e\t%p\n", &global_a, &global_b, &local_c, &local_d, malloc_e);
	
    char command[1024];
	printf("按任意按鍵，印出這個 task 的 memory layout\n");
	getchar();

	// sprintf(command, "pmap %d", getpid());
    sprintf(command, "vmmap %d", getpid()); // (MAC M1)
	assert(system(command)==0);
}
