#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define MAPSIZE 1024*1024*1024l

int main(int argv, char *argc[]) {
	// declare memory allocation
    long size = MAPSIZE * atol(argc[1]);
    int *ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE, -1, 0);
	memset(ptr, 0, size/4);

    sleep(20000);
	exit(0);
}
