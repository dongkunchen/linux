#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

int checkdir(char *path);

int main(int argc, char *argv[])
{
	int n = checkdir(argv[1]);
	printf("n==[%d]\n", n);
}

int checkdir(char *path)
{
	DIR *pDir = opendir(path);
	if (pDir == NULL)
	{
		perror("opendir error");
		return -1;
	}

	int n = 0;
	char sFullPath[1024];

	struct dirent *p = NULL;
	while ((p = readdir(pDir)) != NULL)
	{
		if (strcmp(p->d_name, ".") == 0 || strcmp(p->d_name, "..") == 0)
		{
			continue;
		}

		printf("文件名:[%s/%s]-->", path, p->d_name);
		switch (p->d_type)
		{
		case DT_DIR:
			printf("目錄文件\n");
			memset(sFullPath, 0x00, sizeof(sFullPath));
			sprintf(sFullPath, "%s/%s", path, p->d_name);
			n += checkdir(sFullPath);
			break;
		case DT_REG:
			printf("普通文件\n");
			n++;
			break;
		case DT_LNK:
			printf("鏈接文件\n");
			break;
		default:
			printf("未知文件\n");
		}

		closedir(pDir);

		return n;
	}
}