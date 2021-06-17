//線程同步-互斥鎖
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

//定義一把互斥鎖
pthread_mutex_t mutex;

//定義條件變量
pthread_cond_t cond;

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
        pthread_mutex_lock(&mutex);
        pNode->next = head;
        head = pNode;

        //解鎖
        pthread_mutex_unlock(&mutex);

        //解鎖之後再通知消費者解除阻塞
        pthread_cond_signal(&cond);

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
        pthread_mutex_lock(&mutex);

        if (head == NULL)
        {
            //若條件不滿足需要阻塞等待
            //若條件不滿足則阻塞等帶並解鎖
            //若條件滿足(被生產者線程調用pthread_cond_signal函數通知)解除阻塞並加鎖
            pthread_cond_wait(&cond, &mutex);
        }

        printf("C:[%d]\n", head->data);
        pNode = head;
        head = head->next;

        //解鎖
        pthread_mutex_unlock(&mutex);

        free(pNode);
        pNode = NULL;
        sleep(rand() % 3);
    }
    // pthread_exit(NULL);
}

int main()
{
    int ret;
    pthread_t thread1;
    pthread_t thread2;

    //互斥鎖初始化要在主線程內
    pthread_mutex_init(&mutex, NULL);

    //條件變量初始化
    pthread_cond_init(&cond, NULL);

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

    //釋放互斥鎖
    pthread_mutex_destroy(&mutex);

    //釋放條件變量
    pthread_cond_destroy(&cond);

    return 0;
}