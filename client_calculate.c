/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: client_calculate.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月11日 星期一 15时49分57秒
***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

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

//菜单
void menu(HD *hd, DAT *dat, char *buf);
//解包
void unpack(char *buf);

int main(int argc,const char* argv[])
{
    //创建套接字
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("socket");
        exit(-1);
    }

    //确定服务器IP和端口
    struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = 8080;
    addr.sin_addr.s_addr = inet_addr("192.168.117.128");

    //创建包头和包体
    HD hd;
    memset(&hd, 0, sizeof(HD));
    DAT dat;
    memset(&dat, 0, sizeof(DAT));

    int ret = 0;
    float ans = 0;
    char buf[1024] = {0};

    while(1)
    {
        memset(buf,0,sizeof(buf));
        menu(&hd, &dat, buf);
     
        puts(buf);

        // 发送包到服务器
        ret = sendto(sock, buf, sizeof(HD) + sizeof(DAT), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));
        if(ret == -1)
        {
            perror("sendto");
            exit(-1);
        }

   
        memset(buf,0,sizeof(buf));
        //接受服务器答案
        ret = recvfrom(sock,buf,sizeof(buf),0,NULL,NULL);
        if(ret == -1)
        {
            perror("recvfrom");
            exit(-1);
        }
    
        unpack(buf);
    }

    return 0;
}

//菜单
void menu(HD *hd, DAT *dat, char *buf) {
    printf("请选择运算1-加法\n2-减法\n3-乘法\n4-除法\n5-退出\n");
    scanf("%d", &hd->type);

    // 清空 information 字符串
    memset(hd->information, 0, sizeof(hd->information));

    switch (hd->type) {
        case 1:
            strcpy(hd->information, "add");
            break;
        case 2:
            strcpy(hd->information, "sub");
            break;
        case 3:
            strcpy(hd->information, "mul");
            break;
        case 4:
            strcpy(hd->information, "div");
            break;
        case 5:
            exit(1);
        default:
            printf("无效输入，请选择1-5\n");
            return;
    }

    printf("%d %s\n", hd->type, hd->information);
    printf("请输入想要计算的两个数\n");

    if (scanf("%f %f", &dat->num1, &dat->num2) != 2) {
        printf("输入无效，请重新输入两个浮点数。\n");
        return;
    }

    printf("%f %f\n", dat->num1, dat->num2);

    // 组包
    memcpy(buf, hd, sizeof(HD));

    memcpy(buf + sizeof(HD), dat, sizeof(DAT));
    HD *p = (HD *)buf;
    printf("%d%s\n", p->type, p->information);
}

//解包
void unpack(char *buf)
{
    HD *p = (HD *)buf;
    if (p->type == -1 || p->type == -2)
    {
        printf("%s\n", p->information);
    } else 
    {
        float *ans = (float *)(buf + sizeof(HD));
        printf("answer:%.2f\n", *ans);
    }
}