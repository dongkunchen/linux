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
				printf("普通文件");
				break;
			case DT_DIR:
				printf("目錄文件");
				break;
			case DT_LNK:
				printf("鏈接文件");
				break;
			default:
				printf("未知文件");
		}
		printf("\n");
	}
	closedir(pDir);

	return 0;
}
