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

	printf("%s test start\n", __func__);

	printf("invoke open()\n");
	fd = open(devfile, O_RDWR);
	if (fd < 0) {
		perror("open");
		return NG;
	}

	printf("invoke close()\n");
	close(fd);

	printf("%s test finish\n", __func__);
	return OK;
}

int open_close_test2(const char *devfile)
{
	#define FDNUM 3
	int fd[FDNUM];
	int i;

	printf("%s test start\n", __func__);

	printf("invoke open()\n");
	for (i = 0; i < FDNUM; i++) {
		fd[i] = open(devfile, O_RDWR);
		if (fd[i] < 0) {
			perror("open");
			return NG;
		}
	}
	
	for (i = 0; i < FDNUM; i++) {
		close(fd[i]);
	}

	printf("%s test finish\n", __func__);
	return OK;
}


int read_write_test(const char *devfile)
{
	int fd;
	char string[] = "test!!!!";
	char buffer[sizeof(string)+1];
	int ret;

	printf("%s test start\n", __func__);

	fd = open(devfile, O_RDWR);
	if (fd < 0) {
		perror("open");
		return NG;
	}

	ret = write(fd, string, sizeof(string));
	if (ret != sizeof(string)) {
		printf("write() retval : %d\n", ret);
		return NG;
	}

	lseek(fd, 0, SEEK_SET);

	ret = read(fd, buffer, sizeof(buffer));
	if (ret != sizeof(buffer)) {
		printf("read() retval : %d\n", ret);
		return NG;
	}

	printf("%s\n", buffer);

	close(fd);

	printf("%s test finish\n", __func__);
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

	if (open_close_test2(argv[1]) == NG) {
		return NG;
	}

	if (read_write_test(argv[1]) == NG) {
		return NG;
	}

	return OK;
}

