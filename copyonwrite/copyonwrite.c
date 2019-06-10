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
#define FILEMAP "big"

int main() {
	// declare memory allocation
	int *array = malloc(sizeof(int) * ARRSIZE);
	memset(array, 0, ARRSIZE*sizeof(int));

	// Shared mem
	pid_t child;
	if ((child = fork()) == 0) {
		int devNull = open("/dev/null", O_WRONLY|O_CREAT|O_APPEND);
		int b = write(devNull, array, sizeof(int) * ARRSIZE);
	}
	if ((child = fork()) == 0) {
		int devNull = open("/dev/null", O_WRONLY|O_CREAT|O_APPEND);
		int b = write(devNull, array, sizeof(int) * ARRSIZE);
	}
	if ((child = fork()) == 0) {
		int devNull = open("/dev/null", O_WRONLY|O_CREAT|O_APPEND);
		int b = write(devNull, array, sizeof(int) * ARRSIZE);
	}

	sleep(20000);
	exit(0);
}
