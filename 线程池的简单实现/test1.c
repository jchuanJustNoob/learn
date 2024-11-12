/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: test1.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月05日 星期二 22时58分11秒
***************************************************/

#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>

int flag = 1;

void signal_output(int sig);
//生产者函数
void *input_num(void *arg);
//消费者函数
void *sum_printf(void *arg);


int main(int argc,const char* argv[])
{
    struct threads_of_cus_and_pro cus_pro = {10, 10, 10};

    signal(SIGINT, signal_output);

    struct threadPool *pool = thread_pool_init(cus_pro, input_num, sum_printf);

    for(int i = 0;i < 10;i++)
	{
		sem_post(&pool->sem2);
	}

	for(int i = 0;i < 2;i++)
	{
		sem_post(&pool->sem1);
	}


    thread_pool_destory(pool);

    return 0;
}

void signal_output(int sig)
{
    if (sig == SIGINT)
        flag = 0;
}

//生产者
void *input_num(void *arg)
{
    struct threadPool *p = (struct threadPool*)arg;
    struct numsum data;

    while(flag == 1)
    {
        printf("input data1 and data2\n");
        scanf("%d%d", &data.num1, &data.num2);
        
        sem_wait(&p->sem1);        
        pthread_mutex_lock(&p->mutex);
        push(data, p->que);
        pthread_mutex_unlock(&p->mutex);
        sem_post(&p->sem2);
    }

    return NULL;
}

//消费者
void *sum_printf(void *arg)
{
    struct threadPool *p = (struct threadPool*)arg;
    struct numsum *sum;

    while(flag == 1)
    {
        sem_wait(&p->sem2);
        pthread_mutex_lock(&p->mutex);

        sum = front(p->que);
        if (sum == NULL)
        {
            pthread_mutex_unlock(&p->mutex);
            sem_post(&p->sem1);
            continue;
        }
        
        printf("data1 = %d,data2 = %d sum = %d\n", sum->num1,sum->num2, sum->num1+sum->num2);
        pop(p->que); 
                    
        pthread_mutex_unlock(&p->mutex);
        sem_post(&p->sem1);       
          
    }

    return NULL;
}