#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	struct stat sb;
	stat(argv[1], &sb);
	if((sb.st_mode & S_IFMT) == S_IFREG)
	{
		printf("普通文件\n");
	}
	else if((sb.st_mode & S_IFMT) == S_IFDIR)
	{
		printf("目錄文件\n");
	}
	else if((sb.st_mode & S_IFMT) == S_IFLNK)
	{
		printf("連接文件\n");	
	}

	return 0;

}

