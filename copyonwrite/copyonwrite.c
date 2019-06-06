#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>

// int = 4 bytes
#define ARRSIZE 300*1024*1024
#define FILELOC "big"

int main() {
	// declare memory allocation
	int *array = malloc(sizeof(int) * ARRSIZE);
	//int *array = alloca(sizeof(int) * ARRSIZE);
	// actually allocate the memory
	memset(array, 0, ARRSIZE*sizeof(int));
	// mmap
	int fd = open(FILELOC, O_RDWR);
	struct stat sb;
	fstat(fd, &sb);
	int *ptr = mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	memset(ptr, 0, sb.st_size);

	// Shared mem
	pid_t child;
	if ((child = fork()) == 0) {
		int devNull = open("/dev/null", O_WRONLY);
		int b = write(devNull, ptr, sb.st_size);
		/*
	    for (int i=0; i<sb.st_size/sizeof(int); i++) {
	    	ptr[i] = 1;
	    }
		msync(ptr, sb.st_size, MS_SYNC);
		*/
	}

	sleep(20000);
	exit(0);
}
