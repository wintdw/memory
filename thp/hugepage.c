#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#define MAPSIZE 1024*1024*4     // 1m of int blocks

int main(int argc, char *argv[]) {
    long size = (long)MAPSIZE * (long)atoi(argv[1]);
    int *ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    madvise(ptr, size, MADV_HUGEPAGE);
    memset(ptr, 1, size);

    exit(0);
}
