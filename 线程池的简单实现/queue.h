/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: queue.h
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年11月05日 星期二 17时22分21秒
***************************************************/

#ifndef _QUEUE_H_
#define _QUEUE_H_

struct numsum 
{
    int num1;
    int num2;
};

typedef struct queue
{
    struct numsum add;
    struct queue *next;
}QUE;

//创建一个队列s
QUE *creat_queue();
//放数据（尾插）
void push(struct numsum data, QUE *que);
//返回第一个节点的值
struct numsum *front(QUE *que);
// 删除第一个数据节点
void pop(QUE *que);
//返回队列size
int size(QUE *que);
//判断是否为空
int is_empty(QUE *que); 
//释放队列
void free_queue(QUE *que);

#endif
