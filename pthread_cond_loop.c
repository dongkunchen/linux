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
    int n = *(int *)arg;
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
        printf("P[%d]:[%d]\n", n, pNode->data);

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
    int n = *(int *)arg;
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

        if(head==NULL)
        {
            //一定要解鎖因為上面有加鎖
            pthread_mutex_unlock(&mutex);
            continue;
        }

        printf("C[%d]:[%d]\n", n, head->data);
        pNode = head;
        head = head->next;

        //解鎖
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

    //互斥鎖初始化要在主線程內
    pthread_mutex_init(&mutex, NULL);

    //條件變量初始化
    pthread_cond_init(&cond, NULL);

    for (i = 0; i < 5; i++)
    {
        arr[i] = i;
        //創建生產者線程
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
        //創建消費者線程
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

    //釋放互斥鎖
    pthread_mutex_destroy(&mutex);

    //釋放條件變量
    pthread_cond_destroy(&cond);

    return 0;
}