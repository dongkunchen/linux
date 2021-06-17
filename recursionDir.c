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

		printf("File Name:[%s/%s]-->", path, p->d_name);
		switch (p->d_type)
		{
		case DT_DIR:
			printf("Folder file\n");
			memset(sFullPath, 0x00, sizeof(sFullPath));
			sprintf(sFullPath, "%s/%s", path, p->d_name);
			n += checkdir(sFullPath);
			break;
		case DT_REG:
			printf("Normal file\n");
			n++;
			break;
		case DT_LNK:
			printf("Link file\n");
			break;
		default:
			printf("Unknown file\n");
		}

		closedir(pDir);

		return n;
	}
}
