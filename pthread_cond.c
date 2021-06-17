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
    while (1)
    {
        pNode = (NODE *)malloc(sizeof(NODE));
        if (pNode == NULL)
        {
            perror("malloc error");
            exit(-1);
        }
        pNode->data = rand() % 1000;
        printf("P:[%d]\n", pNode->data);

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
    while (1)
    {
        pthread_mutex_lock(&mutex);

        if (head == NULL)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        printf("C:[%d]\n", head->data);
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
    pthread_t thread1;
    pthread_t thread2;

    pthread_mutex_init(&mutex, NULL);

    pthread_cond_init(&cond, NULL);

    ret = pthread_create(&thread1, NULL, producer, NULL);
    if (ret != 0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }

    ret = pthread_create(&thread2, NULL, consumer, NULL);
    if (ret != 0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&cond);

    return 0;
}
