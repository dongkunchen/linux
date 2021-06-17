#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

typedef struct node
{
    int data;
    struct node *next;
} NODE;

NODE *head = NULL;

pthread_mutex_t mutex;

pthread_cond_t cond;

void *producer(void *arg)
{
    NODE *pNode = NULL;
    int n = *(int *)arg;
    while (1)
    {

        pNode = (NODE *)malloc(sizeof(NODE));
        if (pNode == NULL)
        {
            perror("malloc error");
            exit(-1);
        }
        pNode->data = rand() % 1000;
        printf("P[%d]:[%d]\n", n, pNode->data);

        pthread_mutex_lock(&mutex);
        pNode->next = head;
        head = pNode;

        pthread_mutex_unlock(&mutex);

        pthread_cond_signal(&cond);

        sleep(rand() % 3);
    }
}

void *consumer(void *arg)
{
    NODE *pNode = NULL;
    int n = *(int *)arg;
    while (1)
    {
        pthread_mutex_lock(&mutex);

        if (head == NULL)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        if(head==NULL)
        {
            pthread_mutex_unlock(&mutex);
            continue;
        }

        printf("C[%d]:[%d]\n", n, head->data);
        pNode = head;
        head = head->next;

        pthread_mutex_unlock(&mutex);

        free(pNode);
        pNode = NULL;
        sleep(rand() % 3);
    }
}

int main()
{
    int ret;
    int i = 0;
    int arr[5];
    pthread_t thread1[5];
    pthread_t thread2[5];

    pthread_mutex_init(&mutex, NULL);

    pthread_cond_init(&cond, NULL);

    for (i = 0; i < 5; i++)
    {
        arr[i] = i;
        ret = pthread_create(&thread1[i], NULL, producer, &arr[i]);
        if (ret != 0)
        {
            printf("pthread_create error, [%s]\n", strerror(ret));
            return -1;
        }
    }

    for (i = 0; i < 5; i++)
    {
        arr[i] = i;
        ret = pthread_create(&thread2[i], NULL, consumer, &arr[i]);
        if (ret != 0)
        {
            printf("pthread_create error, [%s]\n", strerror(ret));
            return -1;
        }
    }

    for (i = 0; i < 5; i++)
    {
        pthread_join(thread1[i], NULL);
        pthread_join(thread2[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&cond);

    return 0;
}
