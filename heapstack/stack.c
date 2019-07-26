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

int main() {
    alloc_stack(ONEG);
    alloc_stack(ONEG);

    sleep(20000);
    exit(0);
}
