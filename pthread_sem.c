#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct node
{
    int data;
    struct node *next;
} NODE;

NODE *head = NULL;

sem_t sem_producer;
sem_t sem_consumer;

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

        sem_wait(&sem_producer);
        
        pNode->next = head;
        head = pNode;

        sem_post(&sem_consumer);

        sleep(rand() % 3);
    }
}

void *consumer(void *arg)
{
    NODE *pNode = NULL;
    while (1)
    {
        sem_wait(&sem_consumer); 

        printf("C:[%d]\n", head->data);
        pNode = head;
        head = head->next;

        sem_post(&sem_producer);

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

    sem_init(&sem_producer, 0, 5);
    sem_init(&sem_consumer, 0, 0);

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

    sem_destroy(&sem_producer);
    sem_destroy(&sem_consumer);

    return 0;
}
