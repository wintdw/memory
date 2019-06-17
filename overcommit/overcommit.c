#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ALLOCSZ 1024*1024*1024*6l  // 6G

int main() {
	// declare memory allocation
    int *ptr = mmap(NULL, ALLOCSZ, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE, -1, 0);
	memset(ptr, 0, ALLOCSZ/4);

    sleep(20000);
	exit(0);
}
