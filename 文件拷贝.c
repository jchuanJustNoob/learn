/***************************************************
> Copyright (C) 2024 ==WMH== All rights reserved.
> File Name: 文件拷贝.c
> Author: wmh
> wmh20010626@163.com 
> Created Time: 2024年09月27日 星期五 16时09分07秒
***************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define fileName "./file.txt"
#define cpyFileName "./cpyfile.txt"


int main(int argc,const char* argv[])
{
    char ch = 0;

    FILE* fp = NULL;
    fp = fopen(fileName, "r");

    FILE* cpy_fp = NULL;
    cpy_fp = fopen(cpyFileName, "w");

    if (fp == NULL || cpy_fp == NULL)
    {
        perror("fopen");
        return -1;
    }else 
    {
        printf("文件打开成功\n");
    }

    printf("原文件内容\n");
    while (1)
    {
        ch = fgetc(fp);
        if (ch == -1)
        {
            break;
        }
        printf("%c", ch);
    }

    fclose(fp);
    fp = fopen(fileName, "r");

    while (1)
    {
        ch = fgetc(fp);
        if (ch == -1)
        {
            break;
        }
        fputc(ch, cpy_fp);
    }
    fclose(cpy_fp);
    cpy_fp = fopen(fileName, "r");

    printf("新文件内容\n");
    while(1)
    {
        ch = fgetc(cpy_fp);
        if (ch == -1)
        {
            break;
        }
        printf("%c", ch);
    }

    fclose(fp);
    fclose(cpy_fp);

    return 0;
}
