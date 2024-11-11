/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: server_json.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月10日 星期日 18时38分53秒
***************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cJSON.h"

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

// 创建套接字并绑定服务器
int socket_bind(void);
// 解包 JSON
SEN *json_sever_analysis(char *buf);
// 打包 JSON
char *json_package(SEN *sen);

int main(int argc, const char *argv[])
{
    int sock = socket_bind();

    
    struct sockaddr_in caddr;
    memset(&caddr, 0, sizeof(caddr));

    char buf1[1024] = {0};
    int ret = 0;
    int len = sizeof(caddr);
    SEN *sen = NULL;
    char *buf = NULL;

    while (1)
    {
        memset(buf1, 0, sizeof(buf1));
        ret = recvfrom(sock, buf1, sizeof(buf1), 0, (struct sockaddr *)&caddr, &len);
        if (ret == -1)
        {
            perror("recvfrom");
            exit(-1);
        }

        // 分析接收的 JSON 数据
        sen = json_sever_analysis(buf1);
        if (sen == NULL)
        {
            continue;
        }

        // 打包 JSON 数据
        buf = json_package(sen);
        if (buf == NULL)
        {
            free(sen);
            continue;
        }

        // 发送结果到客户端
        ret = sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&caddr, sizeof(struct sockaddr));
        if (ret == -1)
        {
            perror("sendto");
            exit(-1);
        }

        // 释放内存
        free(sen);
        free(buf); 
    }

    return 0;
}

// 创建套接字并绑定服务器
int socket_bind(void)
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
char *json_package(SEN *sen)
{
    cJSON *json = cJSON_CreateObject();
    if (json == NULL)
    {
        fprintf(stderr, "Failed to create JSON object.\n");
        return NULL;
    }

    cJSON_AddItemToObject(json, "type", cJSON_CreateNumber(sen->type));
    cJSON_AddItemToObject(json, "info", cJSON_CreateString(sen->info));
    cJSON_AddItemToObject(json, "answer", cJSON_CreateNumber(sen->answer));

    char *buf = cJSON_Print(json);
    if (buf == NULL)
    {
        fprintf(stderr, "Failed to print JSON.\n");
    }

    cJSON_Delete(json); 
    return buf;
}
