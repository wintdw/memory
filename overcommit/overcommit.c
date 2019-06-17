#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ALLOCSZ = 1024*1024*1024*6  // 6G

int main() {
	// declare memory allocation
	int *array = malloc(ALLOCSZ);
	memset(array, 0, ARRSIZE*sizeof(int)/4);

	sleep(20000);
	exit(0);
}
