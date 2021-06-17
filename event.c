#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>

int main()
{
    int i = 0;
    //獲取當前系統支持的方法
    const char **p = event_get_supported_methods();
    while (p[i]!=NULL)
    {
        printf("%s \t", p[i++]);
    }
    printf("\n");

    //獲取地基節點
    struct event_base *base = event_base_new();
    if(base==NULL)
    {
        printf("event_base_new error\n");
        return -1;
    }
     
    //獲取當前系統使用的方法
    const char *pp = event_base_get_method(base);
    printf("%s\n", pp);

    //釋放地基節點
    event_base_free(base);
    

    return 0;
    
}