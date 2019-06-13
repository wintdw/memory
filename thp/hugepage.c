#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#define MAPSIZE 500*1024*1024*4     // 2G

int main() {
    int *ptr = mmap(NULL, MAPSIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	madvise(ptr, MAPSIZE, MADV_HUGEPAGE);
	memset(ptr, 1, MAPSIZE);
	
	exit(0);
}
