#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h> 

#define MAPSIZE 1024*1024*1024l

int main(int argc, char *argv[]) {
	// declare memory allocation
    long size = MAPSIZE * atol(argv[1]);
	int *array = malloc(size);
	memset(array, 0, size);

	// Shared mem
	pid_t child;
	if ((child = fork()) == 0) {
		int devNull = open("/dev/null", O_WRONLY|O_CREAT|O_APPEND);
		write(devNull, array, size);
	}
	if ((child = fork()) == 0) {
		int devNull = open("/dev/null", O_WRONLY|O_CREAT|O_APPEND);
		write(devNull, array, size);
	}
	if ((child = fork()) == 0) {
		int devNull = open("/dev/null", O_WRONLY|O_CREAT|O_APPEND);
		write(devNull, array, size);
	}

	sleep(20000);
	exit(0);
}
