/* linux C 用条件变量实现生产者消费者模型 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct MSG{
    struct MSG* next;
    struct MSG* pre;
    int id;
}msg_t;

msg_t *phead = NULL, *ptail = NULL;

// 静态初始化 也可以用pthread_cond_init(&hasproduct, NULL); 进行初始化
pthread_cond_t hasproduct = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// 生产者
void* productor(void *p)
{
    msg_t* mg = NULL;
    while(1){
        pthread_mutex_lock(&lock);
		// 生产产品
        mg = (msg_t*)malloc(sizeof(msg_t));
        scanf("%d", &mg->id);
        printf("product id = %d\n",mg->id);
		// 加入到生产队列里面
        mg->next = NULL;
        mg->pre = ptail;
        ptail->next = mg;
        ptail = mg;

        pthread_mutex_unlock(&lock);
        // 唤醒至少一个阻塞在条件变量上的线程
        pthread_cond_signal(&hasproduct);
    }
    return NULL;
}

// 消费者
void* consumer(void *p)
{
    msg_t* mg = NULL;
    while(1){
        pthread_mutex_lock(&lock);
        while(phead == ptail){ // 注意这里不能是 if 因为会有多个消费者争夺支援
        	// 当队列没有产品的时候会阻塞且释放锁(unlock)直到被唤醒
        	// 有产品被唤醒的时候会返回且重新获得锁(lock)
            pthread_cond_wait(&hasproduct, &lock);
        }
        printf("consume id = %d\n", ptail->id);
		// 消费一个产品
        ptail = ptail->pre;
        free(ptail->next);
        ptail->next = NULL;

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main()
{
    phead = (msg_t*)malloc(sizeof(msg_t));
    ptail = phead;
    pthread_t pid, cid;

    pthread_create(&pid, NULL, productor, NULL);
    pthread_create(&cid, NULL, consumer, NULL);
	
    pthread_join(pid, NULL);
    pthread_join(cid, NULL);

    return 0;
}
