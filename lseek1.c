#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd = open(argv[1], O_RDWR | O_CREAT, 0777);
	if(fd<0)
	{
		perror("open error");
		return -1;
	}
	
	lseek(fd, 100, SEEK_SET);
	printf("file size:[%d]\n", len);

	wirte(fd, "H", 1);

	close(fd);

	return 0;
}
