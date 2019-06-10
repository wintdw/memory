#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>

#define MAPSIZE 1024*1024*1024*2u
#define MAPSIZE2 1024*1024*1024

int main() {
	int *ptr = mmap(NULL, MAPSIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	memset(ptr, 1, MAPSIZE);

	// Shared mem
	pid_t child;
	if ((child = fork()) == 0) {
		memset(ptr, 2, MAPSIZE);
		int devNull = open("/dev/null", O_WRONLY);
		int b = write(devNull, ptr, MAPSIZE);
		int *p = mmap(NULL, MAPSIZE2, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		memset(p, 3, MAPSIZE2);
	}

	sleep(20000);
	exit(0);
}
