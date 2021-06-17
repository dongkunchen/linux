#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

int main (int argc, char *argv[])
{
	DIR *pDir = opendir(argv[1]);
	if(pDir==NULL)
	{
		perror("opendir error");
		return -1;
	}
	struct dirent *pDent = NULL;
	while((pDent=readdir(pDir))!=NULL)
	{
		if(strcmp(pDent->d_name, ".")==0 || strcmp(pDent->d_name, "..")==0)
		{
			continue;
		}
		printf("[%s]------>", pDent->d_name);
		switch(pDent->d_type)
		{
			case DT_REG:
				printf("Normal file");
				break;
			case DT_DIR:
				printf("Folder file");
				break;
			case DT_LNK:
				printf("Link file");
				break;
			default:
				printf("Unknown file");
		}
		printf("\n");
	}
	closedir(pDir);

	return 0;
}
