/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: client_threadpool_calculate.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月13日 星期三 12时40分39秒
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
#include "cJSON.h"

typedef struct calculate
{
    int type;
    char info[20];
    float num1;
    float num2;
} CAL;

typedef struct recv
{
    int type;
    char info[30];
    float answer;
} REC;

//创建套接字
int create_sock();
// 创建 JSON 格式文件
void json_creat(CAL *cal,char *buf);
// 解包 JSON
void json_analysis(char *buf);

int main(int argc,const char* argv[])
{
    int ret = 0;
    char buf[100] = {0};
    CAL cal;
    memset(&cal, 0, sizeof(CAL));

    int sock = create_sock();

    while(1)
    {
        //发送数据
        memset(buf, 0, sizeof(buf));
        json_creat(&cal, buf);

        ret = send(sock, buf, strlen(buf), 0);
        if (ret == -1)
        {
            perror("send");
            exit(-1);
        }

        if (cal.type == 5)
        {
            exit(1);
        }

        ret = recv(sock, buf, sizeof(buf), 0);
        if (ret == -1)
        {
            perror("recv");
            exit(-1);
        }

        json_analysis(buf);
    }

    close(sock);

    return 0;
}

//创建套接字
int create_sock()
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

    ret = connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if (ret == -1)
    {
        perror("connect");
        exit(-1);
    }

    return sock;
}

// 创建 JSON 格式文件
void json_creat(CAL *cal,char *buf)
{
    cJSON *json = cJSON_CreateObject();

    printf("请输入您想要计算的数\n");
    scanf("%f%f", &cal->num1, &cal->num2);

    printf("请选择运算\n1------'+'\n2------'-'\n3------'*'\n4------'/'\n5------'quit'\n");
    scanf("%d", &cal->type);

    cJSON_AddItemToObject(json, "type", cJSON_CreateNumber(cal->type));
    if (cal->type == 1)
    {
        strcpy(cal->info, "add");
    }
    else if (cal->type == 2)
    {
        strcpy(cal->info, "sub");
    }
    else if (cal->type == 3)
    {
        strcpy(cal->info, "mul");
    }
    else if (cal->type == 4)
    {
        strcpy(cal->info, "div");
    }

    cJSON_AddItemToObject(json, "info", cJSON_CreateString(cal->info));
    cJSON_AddItemToObject(json, "num1", cJSON_CreateNumber(cal->num1));
    cJSON_AddItemToObject(json, "num2", cJSON_CreateNumber(cal->num2));

    memcpy(buf, cJSON_Print(json), strlen(cJSON_Print(json)));

    cJSON_Delete(json);
}

// 解析 JSON 数据
void json_analysis(char *buf)
{
    REC rec;
    memset(&rec, 0, sizeof(REC));

    cJSON *json = cJSON_Parse(buf);
    if (json == NULL)
    {
        printf( "parse JSON error\n");
        return;
    }

    cJSON *node = cJSON_GetObjectItem(json, "type");
    if (node != NULL && node->type == cJSON_Number)
    {
        rec.type = node->valueint;
    }
    else
    {
        printf("type not found\n");
        cJSON_Delete(json);
        return;
    }

    node = cJSON_GetObjectItem(json, "info");
    if (node != NULL && node->type == cJSON_String)
    {
        strcpy(rec.info, node->valuestring);
    }
    else
    {
        printf("info not found\n");
        cJSON_Delete(json);
        return;
    }

    if (rec.type == -1 || rec.type == -2)
    {
        printf("%s\n", rec.info);
    }
    else
    {
        node = cJSON_GetObjectItem(json, "answer");
        if (node != NULL && node->type == cJSON_Number)
        {
            rec.answer = node->valuedouble;
        }
        printf("%s\n", rec.info);
        printf("answer: %.2f\n", rec.answer);
    }

    cJSON_Delete(json); 
}
