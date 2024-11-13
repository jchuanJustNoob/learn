/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: queue.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月13日 星期三 12时41分33秒
***************************************************/

#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <assert.h>

//创建头
QUE *creat_queue()
{
    QUE *head = (QUE*)malloc(sizeof(QUE));
    if(head == NULL)
    {
        printf("Error: malloc failed!\n");
        exit(1);
    }
    memset(head, 0, sizeof(QUE));
    head->next = NULL;
    return head;
}

//放数据（尾插）
void push(int sock, QUE *que)
{
    QUE *in_queue = (QUE *)malloc(sizeof(QUE));

    while(que->next != NULL)
    {
        que = que->next;
    }

    in_queue->sock = sock;
    in_queue->next = NULL;

    que->next = in_queue;
}

//查看第一个数据节点数据
int front(QUE *que)
{
    assert(que != NULL);

    if (que->next == NULL)
        return -1;  // 如果队列为空，返回NULL

    QUE *read_queue = que->next;

    int p = read_queue->sock;

    return p;
}

//删除第一个数据节点
void pop(QUE *que)
{
    assert(que!= NULL);

    if (que->next == NULL)
        return;
        
    QUE *delete_queue = que->next;

    que->next = delete_queue->next;
    free(delete_queue);
}

//返回队列size
int size(QUE *que)
{
    assert(que!= NULL);
    int lenth = 1;
    
    QUE *read_queue = que->next;
    while(read_queue->next != NULL)
    {
        read_queue = read_queue->next;
        lenth++;
    }

    return lenth; 
}

//判断是否为空
int is_empty(QUE *que)
{
    int flag = 0;
    if(que->next == NULL)
        flag = -1;
    return flag;
}

//释放队列
void free_queue(QUE *que)
{
    QUE *tmp = que;
    while(que != NULL)
    {
        que = tmp->next;
        free(tmp);
        tmp = que;
    }
}
