//使用信號量實現生產者和消費者模型
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

//定義信號量
sem_t sem_producer;
sem_t sem_consumer;

//生產者線程
void *producer(void *arg)
{

    NODE *pNode = NULL;
    while (1)
    {
        //生產一個節點
        pNode = (NODE *)malloc(sizeof(NODE));
        if (pNode == NULL)
        {
            perror("malloc error");
            exit(-1);
        }
        pNode->data = rand() % 1000;
        printf("P:[%d]\n", pNode->data);

        //加鎖
        sem_wait(&sem_producer);//相當於--
        
        pNode->next = head;
        head = pNode;

        //解鎖
        sem_post(&sem_consumer);//相當於++

        sleep(rand() % 3);
    }
}

//消費者線程
void *consumer(void *arg)
{
    NODE *pNode = NULL;
    while (1)
    {
        //加鎖
        sem_wait(&sem_consumer); //相當於--

        printf("C:[%d]\n", head->data);
        pNode = head;
        head = head->next;

        //解鎖
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

    //初始化信號量
    sem_init(&sem_producer, 0, 5);
    sem_init(&sem_consumer, 0, 0);

    //創建生產者線程
    ret = pthread_create(&thread1, NULL, producer, NULL);
    if (ret != 0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }

    //創建消費者線程
    ret = pthread_create(&thread2, NULL, consumer, NULL);
    if (ret != 0)
    {
        printf("pthread_create error, [%s]\n", strerror(ret));
        return -1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    //釋放信號量資源
    sem_destroy(&sem_producer);
    sem_destroy(&sem_consumer);

    return 0;
}