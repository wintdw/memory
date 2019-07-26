#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>

#define ONEG 1024*1024*1024l

int main(int argc, char *argv[]) {
	int fd = open(argv[1], O_RDWR);
	struct stat sb;
	fstat(fd, &sb);
	int *ptr = mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FILE, fd, 0);
	memset(ptr, 1, sb.st_size);

	// Shared mem
	pid_t child;
	if ((child = fork()) == 0) {
		memset(ptr, 2, sb.st_size);
		int *p = mmap(NULL, ONEG, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		memset(p, 3, ONEG);
	}

	sleep(20000);
	exit(0);
}
