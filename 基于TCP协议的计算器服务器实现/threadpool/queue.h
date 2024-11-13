/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: queue.h
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月13日 星期三 12时41分28秒
***************************************************/

#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct queue
{
    int sock;
    struct queue *next;
}QUE;

//创建一个队列s
QUE *creat_queue();
//放数据（尾插）
void push(int sock, QUE *que);
//返回第一个节点的值
int front(QUE *que);
// 删除第一个数据节点
void pop(QUE *que);
//返回队列size
int size(QUE *que);
//判断是否为空
int is_empty(QUE *que); 
//释放队列
void free_queue(QUE *que);

#endif
