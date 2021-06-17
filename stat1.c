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
		printf("Normal file\n");
	}
	else if((sb.st_mode & S_IFMT) == S_IFDIR)
	{
		printf("Folder file\n");
	}
	else if((sb.st_mode & S_IFMT) == S_IFLNK)
	{
		printf("L\n");	
	}

	return 0;

}

