#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
int main(int argc, char *argv[])
{
    struct dirent **namelist;
    int n;

    n = scandir(argv[1], &namelist, NULL, alphasort);
    // n = scandir(argv[1], &namelist, NULL, alphasort);//代替執行./scandir .(.代表傳入文件夾名)
    if (n < 0)
    {
        perror("scandir");
    }
    else
    {
        while (n--)
        {
            printf("%s\n", namelist[n]->d_name);
            free(namelist[n]);
        }
        free(namelist);
            
    }
    return 0;
}