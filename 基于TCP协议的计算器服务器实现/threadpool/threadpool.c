/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: threadpool.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月13日 星期三 12时41分08秒
***************************************************/

#include "threadpool.h"

// 线程池创建以及初始化函数
struct threadpool *create_init_threadpool(TAQ taq,void *(*fun)(void*))
{
    //初始化
    struct threadpool *pool = (struct threadpool*)malloc(sizeof(struct threadpool));
    pool->tids = (pthread_t *)malloc(sizeof(pthread_t) * taq.threads_of_customer);

    pool->taq.queue_max_length = taq.queue_max_length;
    pool->taq.threads_of_customer = taq.threads_of_customer;

    sem_init(&pool->sem1, 0, taq.queue_max_length);
    sem_init(&pool->sem2, 0, 0);

    pthread_mutex_init(&pool->mutex, NULL);

    //创建队列
    pool->que = creat_queue();


    //创建消费者线程
    int ret = 0;

    for (int i = 0; i < taq.threads_of_customer;i++)
    {
        ret = pthread_create(&(pool->tids[i]), NULL, fun, pool);
        if (ret != 0)
        {
            printf("thread %d create error\n", i + 1);
            exit(-1);
        }
    }

    return pool;
}

//线程池回收资源
void destroy_threadpool(struct threadpool *pool)
{
    //回收消费者资源
    for (int i = 0; i < pool->taq.threads_of_customer;i++)
    {
        pthread_join(pool->tids[i], NULL);
    }

    //释放消费者线程池id
    free(pool->tids);
    pool->tids = NULL;

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