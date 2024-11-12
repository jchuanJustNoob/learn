/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: threadpool.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月05日 星期二 17时21分11秒
***************************************************/

#include "threadpool.h"
// 初始化
struct threadPool *thread_pool_init(struct threads_of_cus_and_pro cus_pro,  void *(*producer)(void *), void *(*customer)(void *))
{
    //申请空间
    struct threadPool *p = (struct threadPool *)malloc(sizeof(struct threadPool));
    memset(p, 0, sizeof(struct threadPool));

    //初始化信号量以及锁
    sem_init(&p->sem1, 0, cus_pro.queue_max_length);
    sem_init(&p->sem2, 0, 0);

    pthread_mutex_init(&p->mutex, NULL);

    //创建队列，初始化缓冲区
    p->que = creat_queue();
    p->cus_pro.queue_max_length = cus_pro.queue_max_length;
    p->cus_pro.threads_of_customer = cus_pro.threads_of_customer;
    p->cus_pro.threads_of_producer = cus_pro.threads_of_producer;

    int ret = 0;
    // 创建生产者线程
    p->tids_pro = (pthread_t *)malloc(sizeof(pthread_t) * cus_pro.threads_of_producer);

    for (int i = 0; i < cus_pro.threads_of_producer;i++)
    {
        ret = pthread_create(&p->tids_pro[i], NULL,producer,p);

        if(ret != 0)
        {
            printf("producer thread error \n");
            return NULL;
        }
    }

    //创建消费者线程
    p->tids_cus = (pthread_t *)malloc(sizeof(pthread_t) * cus_pro.threads_of_customer);
    

    for (int i = 0; i < cus_pro.threads_of_customer;i++)
    {
        ret = pthread_create(&p->tids_cus[i], NULL, customer, p);

        if (ret != 0)
        {
            printf("customer thread error \n");
            return NULL;
        }
    }

    return p;
}


//销毁
void thread_pool_destory(struct threadPool *pool)
{
    //回收生产者资源
    for (int i = 0; i < pool->cus_pro.threads_of_producer;i++)
    {
        pthread_join(pool->tids_pro[i], NULL);
    }

    //回收消费者资源
    for (int i = 0; i < pool->cus_pro.threads_of_customer;i++)
    {
        pthread_join(pool->tids_cus[i], NULL);
    }

    //释放生产者线程池id
    free(pool->tids_pro);
    pool->tids_pro = NULL;

    //释放消费者线程池id
    free(pool->tids_cus);
    pool->tids_cus = NULL;

    //释放队列
    free_queue(pool->que);

    //释放信号量和锁
    sem_destroy(&pool->sem1);
    sem_destroy(&pool->sem2);
    pthread_mutex_destroy(&pool->mutex);

    //释放
    free(pool);
    pool = NULL;
}