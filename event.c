#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>

int main()
{
    int i = 0;
    const char **p = event_get_supported_methods();
    while (p[i]!=NULL)
    {
        printf("%s \t", p[i++]);
    }
    printf("\n");

    struct event_base *base = event_base_new();
    if(base==NULL)
    {
        printf("event_base_new error\n");
        return -1;
    }
     
    const char *pp = event_base_get_method(base);
    printf("%s\n", pp);

    event_base_free(base);
    

    return 0;
    
}
