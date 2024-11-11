/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: client_json.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月10日 星期日 18时38分36秒
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

typedef struct recv
{
    int type;
    char info[30];
    float answer;
} REC;

// 创建 JSON 格式文件
char *json_creat(CAL *cal);
// 解包 JSON
void json_analysis(char *buf);

int main(int argc, const char *argv[])
{
    // 创建套接字
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("socket");
        exit(-1);
    }

    // 确定服务器 IP 和端口
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = 8080;
    addr.sin_addr.s_addr = inet_addr("192.168.117.128");

    CAL cal;
    char *buf = NULL;
    char buf1[1024] = {0};
    int ret = 0;

    while (1)
    {
        buf = json_creat(&cal);
        if (cal.type == 5)
        {
            free(buf);
            break;
        }

        // 发送包到服务器
        ret = sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));
        if (ret == -1)
        {
            perror("sendto");
            free(buf);
            exit(-1);
        }

        memset(buf1, 0, sizeof(buf1));
        free(buf);

        // 接受服务器返回的包
        ret = recvfrom(sock, buf1, sizeof(buf1), 0, NULL, NULL);
        if (ret == -1)
        {
            perror("recvfrom");
            exit(-1);
        }

        json_analysis(buf1);
    }

    close(sock);
    return 0;
}

// 创建 JSON 格式文件
char *json_creat(CAL *cal)
{
    cJSON *json = cJSON_CreateObject();

    printf("请输入您想要计算的数\n");
    scanf("%f%f", &cal->num1, &cal->num2);

    printf("请选择运算\n1------'+'\n2------'-'\n3------'*'\n4------'/'\n5------'quit'\n");
    scanf("%d", &cal->type);

    cJSON_AddItemToObject(json, "type", cJSON_CreateNumber(cal->type));
    if (cal->type == 1)
    {
        strcpy(cal->info, "addition");
    }
    else if (cal->type == 2)
    {
        strcpy(cal->info, "subtraction");
    }
    else if (cal->type == 3)
    {
        strcpy(cal->info, "multiplication");
    }
    else if (cal->type == 4)
    {
        strcpy(cal->info, "division");
    }

    cJSON_AddItemToObject(json, "info", cJSON_CreateString(cal->info));
    cJSON_AddItemToObject(json, "num1", cJSON_CreateNumber(cal->num1));
    cJSON_AddItemToObject(json, "num2", cJSON_CreateNumber(cal->num2));

    char *buf = cJSON_Print(json);
    cJSON_Delete(json); 

    return buf;
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
