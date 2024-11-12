/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: threadpool.h
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月05日 星期二 17时21分17秒
***************************************************/

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "queue.h"
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct threads_of_cus_and_pro
{
    int queue_max_length;//队列的最大长度
    int threads_of_customer;//有几个消费者线程
    int threads_of_producer;//有几个生产者线程
};

struct threadPool
{
    sem_t sem1;
    sem_t sem2;
    pthread_mutex_t mutex;

    QUE *que;

    struct threads_of_cus_and_pro cus_pro;//消费者和生产者线程数

    pthread_t *tids_pro;//生产者线程id指针，创建数组使用
    pthread_t *tids_cus;//消费者线程id指针，创建数组使用
};

// 初始化
struct threadPool *thread_pool_init(struct threads_of_cus_and_pro cus_pro,  void *(*producer)(void *), void *(*customer)(void *));
//销毁
void thread_pool_destory(struct threadPool *pool);

#endif
