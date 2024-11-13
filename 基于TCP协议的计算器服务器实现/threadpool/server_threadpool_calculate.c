/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: server_threadpool_calculate.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月13日 星期三 12时40分57秒
***************************************************/

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
#include "cJSON.h"
#include "queue.h"
#include "threadpool.h"

typedef struct calculate
{
    int type;
    char info[20];
    float num1;
    float num2;
} CAL;

typedef struct send
{
    int type;
    char info[30];
    float answer;
} SEN;

//创建套接字并监听
int create_sock_listen();
// 解包 JSON
SEN *json_sever_analysis(char *buf);
// 打包 JSON
void json_package(SEN *sen,char *buf);
//生产者函数
void *producer(void *arg);
// 消费者函数
void *customer(void *arg);

int main(int argc,const char* argv[])
{
    TAQ taq = {5, 10};
    struct threadpool *pool = create_init_threadpool(taq, customer);

    pthread_t tid = 0;
    int ret = pthread_create(&tid, NULL, producer, pool);
    if (ret != 0)
    {
        printf("producer thread create error\n");
        exit(-1);
    }

    pthread_join(tid, NULL);

    destroy_threadpool(pool);

    return 0;
}

//生产者函数
void *producer(void *arg)
{
    struct threadpool *pool = (struct threadpool *)arg;
    int sockfd = 0;

    int sock = create_sock_listen();

    while(1)
    {
        sockfd = accept(sock, NULL, NULL);
        if(sockfd == -1)
        {
            perror("accept");
            exit(1);
        }

        sem_wait(&pool->sem1);
        pthread_mutex_lock(&pool->mutex);
        push(sockfd,pool->que);
        pthread_mutex_unlock(&pool->mutex);
        sem_post(&pool->sem2);
    }
    close(sock);
}

//消费者函数
void *customer(void *arg)
{
    struct threadpool *pool = (struct threadpool *)arg;
    int sockfd = 0;

    while (1)
    {
        sem_wait(&pool->sem2);
        pthread_mutex_lock(&pool->mutex);

        sockfd = front(pool->que);
        printf("%d\n", sockfd);
        pop(pool->que);

        pthread_mutex_unlock(&pool->mutex);
        sem_post(&pool->sem1);

        int ret = 0;
        char buf[100] = {0};

        SEN *sen = (SEN *)malloc(sizeof(SEN));
        memset(sen, 0, sizeof(SEN));

        while(1)
        {
            //接收
            memset(buf, 0, sizeof(buf));
            ret = recv(sockfd, buf, sizeof(buf), 0);
            printf("%d\n", sockfd);
            if (ret == -1)
            {
                perror("recv");
                exit(-1);
            }else if (ret = 0)
            {
                close(sockfd);
                exit(1);
            }

            sen = json_sever_analysis(buf);
            if (sen->type == -3)
            {
                printf("%d quit\n", sockfd);
                break;
            }


            memset(buf, 0, sizeof(buf));

            json_package(sen, buf);
            // 发送数据
            ret = send(sockfd, buf, strlen(buf), 0);
            if (ret == -1)
            {
                perror("send");
                exit(-1);
            } 
        }
    close(sockfd);
    }
}

//创建套接字并监听
int create_sock_listen()
{
    int ret = 0;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = 8888;
    addr.sin_addr.s_addr = inet_addr("192.168.117.128");

    ret = bind(sock,(struct sockaddr *)&addr,sizeof(struct sockaddr));
    if (ret == -1)
    {
        perror("bind");
        exit(-1);
    }

    //监听
    ret = listen(sock, 3);
    if (ret == -1)
    {
        perror("listen");
        exit(-1);
    }

    return sock;
}

SEN *json_sever_analysis(char *buf)
{
    cJSON *json = cJSON_Parse(buf);
    if (json == NULL)
    {
        printf("JSON parse error\n");       
        return NULL;
    }

    CAL cal;
    memset(&cal, 0, sizeof(CAL));

    SEN *sen = (SEN *)malloc(sizeof(SEN));
    if (sen == NULL)
    {
        perror("malloc");
        cJSON_Delete(json);
        return NULL;
    }
    memset(sen, 0, sizeof(SEN));

    cJSON *node = cJSON_GetObjectItem(json, "type");
    if (node == NULL || node->type != cJSON_Number)
    {
        printf("type not found or invalid\n");
        sen->type = -1;
        strcpy(sen->info, "Invalid type");
        cJSON_Delete(json);
        return sen;
    }
    cal.type = node->valueint;

    node = cJSON_GetObjectItem(json, "num1");
    if (node == NULL || node->type != cJSON_Number)
    {
        printf("num1 not found or invalid\n");
        sen->type = -1;
        strcpy(sen->info, "Invalid num1");
        cJSON_Delete(json);
        return sen;
    }
    cal.num1 = node->valuedouble;

    node = cJSON_GetObjectItem(json, "num2");
    if (node == NULL || node->type != cJSON_Number)
    {
        printf("num2 not found or invalid\n");
        sen->type = -1;
        strcpy(sen->info, "Invalid num2");
        cJSON_Delete(json);
        return sen;
    }
    cal.num2 = node->valuedouble;

    strcpy(sen->info, "success");
    if (cal.type == 1)
    {
        sen->type = 1;
        sen->answer = cal.num1 + cal.num2;
    }
    else if (cal.type == 2)
    {
        sen->type = 2;
        sen->answer = cal.num1 - cal.num2;
    }
    else if (cal.type == 3)
    {
        sen->type = 3;
        sen->answer = cal.num1 * cal.num2;
    }
    else if (cal.type == 4)
    {
        if (cal.num2 == 0)
        {
            sen->type = -2;
            strcpy(sen->info, "The divisor cannot be 0");
        }
        else
        {
            sen->type = 4;
            sen->answer = cal.num1 / cal.num2;
        }
    }else if (cal.type == 5)
    {
        sen->type = -3;
    }
    else
    {
        sen->type = -1;
        strcpy(sen->info, "Unknown operation");
    }

    cJSON_Delete(json);
    return sen;
}


// 打包 JSON
void json_package(SEN *sen,char *buf)
{
    cJSON *json = cJSON_CreateObject();
    if (json == NULL)
    {
        printf("Failed to create JSON object.\n");
        return;
    }

    cJSON_AddItemToObject(json, "type", cJSON_CreateNumber(sen->type));
    cJSON_AddItemToObject(json, "info", cJSON_CreateString(sen->info));
    cJSON_AddItemToObject(json, "answer", cJSON_CreateNumber(sen->answer));

    memcpy(buf, cJSON_Print(json), strlen(cJSON_Print(json)));

    if (buf == NULL)
    {
        printf("Failed to print JSON.\n");
    }

    cJSON_Delete(json);   
}