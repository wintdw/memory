#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alloca.h>

// int = 4 bytes
#define ARRSIZE 300*1024*1024
#define FILELOC "big"

// stack limit for system is 8192K
int *alloc_stack(int size) {
	int *alloc = alloca(sizeof(int) * size);
	memset(alloc, 1, sizeof(int) * size);
	return alloc;
}

int stack_cal(int n, int m) {
	int sum = 0;
	int mat[n][m];
	for (int i=0; i<n; i++) {
		for (int j=0; j<m; j++) {
			mat[i][j] = 1;
			sum += mat[i][j];
		}
	}
	return sum;
}

int main() {
	{
		alloc_stack(ARRSIZE);
	}
	{
		alloc_stack(ARRSIZE);
	}
	{
		printf("%d\n", stack_cal(300, 1000000));
	}
	printf("Sleeping\n");
	sleep(20000);
	exit(0);
}
