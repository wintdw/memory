#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ALLOCSZ 1024*1024*1024*6l  // 6G

int main() {
	// declare memory allocation
	int *array = malloc(ALLOCSZ);
	memset(array, 0, ALLOCSZ/4);

	exit(0);
}
