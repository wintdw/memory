#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ONEG 1024*1024*1024l

int *alloc_heap(long size) {
	int *alloc = malloc(size);
	memset(alloc, 1, size);
	return alloc;
}

int leak_func() {
	alloc_heap(ONEG);
	alloc_heap(ONEG);
}

int main() {
	leak_func();
	sleep(20000);
	exit(0);
}
