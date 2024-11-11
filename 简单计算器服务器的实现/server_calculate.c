/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: server_calculate.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月11日 星期一 15时50分18秒
***************************************************/

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>


typedef struct
{
    sem_t sem1;
    sem_t sem2;

    pthread_t tid1;
    pthread_t tid2;
    
    pthread_mutex_t mutex;

    int sock;

    QUE *que;
}THR;

typedef struct head
{
    int type;
    char information[30];
}HD;

typedef struct data
{
    float num1;
    float num2;
} DAT;

//获取缓冲区数据，入队（生产者）
void *producer(void *arg);
//解析队列数据，发送到客户端（消费者）
void *customer(void *arg);
//确定服务器，创建套接字
int create_socket(void);
//初始化函数
void init_thr(THR *thr);
//创建生产者和消费者线程
void creat_pro_cus(void *(*fun1)(void *), void *(*fun2)(void *), THR* thr);
//回收资源
void destroy_thr(THR *thr);

int main(int argc,const char* argv[])
{
    THR *thr = (THR *)malloc(sizeof(THR));
    memset(thr, 0, sizeof(THR));

    init_thr(thr);

    creat_pro_cus(producer, customer, thr);

    destroy_thr(thr);

    return 0;
}

int create_socket(void)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = 8080; 
    addr.sin_addr.s_addr = inet_addr("192.168.117.128");

    int ret = bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if (ret == -1)
    {
        perror("bind");
        exit(-1);
    }

    return sock;
}

//初始化函数
void init_thr(THR *thr)
{
    sem_init(&thr->sem1, 0, 1);
    sem_init(&thr->sem2, 0, 0);

    pthread_mutex_init(&thr->mutex, NULL);

    thr->que = creat_queue();

    thr->sock = create_socket();
}

//创建生产者和消费者线程
void creat_pro_cus(void *(*fun1)(void *), void *(*fun2)(void *), THR* thr)
{
    pthread_t tid1 = 0, tid2 = 0;

    pthread_create(&tid1, NULL, fun1, thr);

    pthread_create(&tid2, NULL, fun2, thr);

    thr->tid1 = tid1;
    thr->tid2 = tid2;

}

//回收资源
void destroy_thr(THR *thr)
{
    pthread_join(thr->tid1, NULL);
    pthread_join(thr->tid2, NULL);

    sem_destroy(&thr->sem1);
    sem_destroy(&thr->sem2);

    pthread_mutex_destroy(&thr->mutex);

    free_queue(thr->que);

    close(thr->sock);

    free(thr);
}

//获取缓冲区数据，入队（生产者）
void *producer(void *arg)
{
    THR *thr = (THR *)arg;
    char buf[1024] = {0};
    int count = 0;

    struct sockaddr_in caddr;
	memset(&caddr,0,sizeof(caddr));

    struct information temp;
    memset(&temp,0,sizeof(temp));

    int len = sizeof(struct sockaddr_in);

    while (1)
    {
  
        sem_wait(&thr->sem1);
        pthread_mutex_lock(&thr->mutex);
  
        count = recvfrom(thr->sock, buf, sizeof(buf), 0, (struct sockaddr *)&caddr, &len);
        if (count == -1)
        {
            perror("recvfrom");
            exit(-1);
        } 
        memmove(temp.mes, buf, count);
        temp.addr = caddr;  
        push(temp, thr->que, count); 

        pthread_mutex_unlock(&thr->mutex);
        sem_post(&thr->sem2);
        
        // HD *p = (HD *)buf;
        // printf("%d%s\n", p->type, p->information);
        // DAT *d = (DAT *)(buf + sizeof(HD));
        // printf("%f%f\n", d->num1, d->num2);

        // memmove(temp.mes, buf, count);
        // printf("%d\n", __LINE__);
        // push(temp, thr->que, count);
        // printf("%d\n", __LINE__);
    }

}

//解析队列数据，发送到客户端（消费者）
void *customer(void *arg)
{
    THR *thr = (THR *)arg;
    struct information *temp = (struct information *)malloc(sizeof(struct information));
    memset(temp, 0, sizeof(temp));
    int ret = 0;
    float answer = 0;
    char buf[200] = {0};

    while (1)
    {
        sem_wait(&thr->sem2);
        pthread_mutex_lock(&thr->mutex);

        if(is_empty(thr->que) == -1)
        {
            pthread_mutex_unlock(&thr->mutex);
            sem_post(&thr->sem1);
            continue;
        }

        temp = front(thr->que);
        // HD *q = (HD *)(temp->mes);
        // printf("%d%s\n", q->type, q->information);
        // DAT *d = (DAT *)((temp->mes) + sizeof(HD));
        // printf("%f%f\n", d->num1, d->num2);
        struct information dest = *temp;
        pop(thr->que);

        pthread_mutex_unlock(&thr->mutex);
        sem_post(&thr->sem1);

        HD *p = (HD *)(dest.mes);
        printf("%d\n", p->type);
        if (p->type == 1)
        {
            DAT *d = (DAT *)(temp->mes + sizeof(HD));
            answer = d->num1 + d->num2;
        }else if(p->type == 2)
        {
            DAT *d = (DAT *)(temp->mes + sizeof(HD));
            answer = d->num1 - d->num2;
        }else if(p->type == 3)
        {
            DAT *d = (DAT *)(temp->mes + sizeof(HD));
            answer = d->num1 * d->num2;
        }else if(p->type == 4)
        {
            DAT *d = (DAT *)(temp->mes + sizeof(HD));
            if(d->num2 == 0)
            {
                p->type = -2;
                strcpy(p->information, "The divisor is not 0");
            }
            answer = d->num1 / d->num2;
        }else 
        {
            p->type = -1;
            strcpy(p->information, "Error type");
        }

        memcpy(buf, p, sizeof(HD));
        memcpy(buf + sizeof(HD), &answer, sizeof(float));

        ret = sendto(thr->sock, buf, sizeof(HD) + sizeof(float), 0, (struct sockaddr *)&(dest.addr), sizeof(struct sockaddr));
        if(ret == -1)
        {
            perror("sento");
            exit(-1);
        }
    }
}