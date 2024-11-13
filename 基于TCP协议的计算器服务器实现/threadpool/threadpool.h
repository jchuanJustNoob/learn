/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: threadpool.h
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月13日 星期三 12时41分16秒
***************************************************/

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "queue.h"
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct threads_and_queue_length
{
    int queue_max_length;//队列的最大长度
    int threads_of_customer;//有几个消费者线程
} TAQ;

struct threadpool
{
    sem_t sem1;
    sem_t sem2;
    pthread_mutex_t mutex;

    QUE *que;
    TAQ taq;

    pthread_t *tids;
};

// 线程池创建以及初始化函数
struct threadpool *create_init_threadpool(TAQ taq,void *(*fun)(void*));
//线程池回收资源
void destroy_threadpool(struct threadpool *pool);

#endif
