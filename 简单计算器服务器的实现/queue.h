/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: queue.h
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月11日 星期一 15时32分03秒
***************************************************/

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <sys/socket.h>
#include <netinet/in.h>

struct information
{
    char mes[200];
    struct sockaddr_in addr;
};

typedef struct queue
{
    struct information data;
    struct queue *next;
}QUE;

//创建一个队列s
QUE *creat_queue();
//放数据（尾插）
void push(struct information data, QUE *que,ssize_t len);
//返回第一个节点的值
struct information *front(QUE *que);
// 删除第一个数据节点
void pop(QUE *que);
//返回队列size
int size(QUE *que);
//判断是否为空
int is_empty(QUE *que); 
//释放队列
void free_queue(QUE *que);

#endif
