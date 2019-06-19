#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alloca.h>
#include <string.h>

#define ONEG 1024*1024*1024l

// stack limit for system is 8192K
int *alloc_stack(long size) {
	int *alloc = alloca(size);
	memset(alloc, 1, size);
	return alloc;
}

int stack_cal(long n, long m) {
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
    alloc_stack(ONEG);
    alloc_stack(ONEG);
    printf("%d\n", stack_cal(1024, 1024*1024));

    sleep(20000);
    exit(0);
}
