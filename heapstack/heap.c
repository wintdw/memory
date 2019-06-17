#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// int = 4 bytes

int *alloc_heap(int size) {
	int *alloc = malloc(sizeof(int) * size);
	memset(alloc, 1, sizeof(int) * size);
	return alloc;
}

int leak_func() {
	alloc_heap(100000000);
	alloc_heap(100000000);
}

int main() {
	leak_func();
	sleep(20000);
	exit(0);
}
