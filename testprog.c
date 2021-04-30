#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define OK 0
#define NG 1

int open_close_test(const char *devfile)
{
	int fd;

	printf("invoke open()\n");
	fd = open(devfile, O_RDWR);
	if (fd < 0) {
		perror("open");
		return NG;
	}

	printf("invoke close()\n");
	close(fd);

	return OK;
}

/* argv[1]: device file (ex. /dev/chardev) */
int main(int argc, char *argv[])
{

	if (argc != 2) {
		printf("invalid arg num. bye/~~");
		return NG;
	}

	if (open_close_test(argv[1]) == NG) {
		return NG;
	}

	return OK;
}

