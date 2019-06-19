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
#define MAPSIZE 1024*1024*1024l

int main() {
	// declare memory allocation
    long size = MAPSIZE * atol(argv[1]);
	int *array = malloc(size);
	memset(array, 0, size);

	// Shared mem
	pid_t child;
	if ((child = fork()) == 0) {
		int devNull = open("/dev/null", O_WRONLY|O_CREAT|O_APPEND);
		int b = write(devNull, array, size);
	}
	if ((child = fork()) == 0) {
		int devNull = open("/dev/null", O_WRONLY|O_CREAT|O_APPEND);
		int b = write(devNull, array, size);
	}
	if ((child = fork()) == 0) {
		int devNull = open("/dev/null", O_WRONLY|O_CREAT|O_APPEND);
		int b = write(devNull, array, size);
	}

	sleep(20000);
	exit(0);
}
