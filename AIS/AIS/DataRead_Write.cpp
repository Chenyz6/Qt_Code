#include "stdafx.h"
#include"DataRead_Write.h"
#include<stdio.h>

DWORD GetFilelen(char* file, int s_len)
{
	FILE *fp;
	fp = fopen(file, "rb");
	if (!fp)
	{
		printf("文件打开错误 请检查文件地址！");
	}
	DWORD size;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp) / s_len;		
	rewind(fp);
	fclose(fp);
	return size;
}

short* GetFileshort(char* file, DWORD size)
{
	FILE *fp;
	fp = fopen(file, "rb");  
	if (!fp)
	{
		printf("文件打开错误 请检查文件地址！");
	}
	short *a;
	a = new short[size];
	fread(a, sizeof(short), size, fp);
	fclose(fp);
	return a;
}

unsigned char* GetFilechar(char* file, DWORD size)
{
	FILE *fp;
	fp = fopen(file, "rb");
	if (!fp)
	{
		printf("文件打开错误 请检查文件地址！");
	}
	unsigned char *a;
	a = new unsigned char[size];
	fread(a, sizeof(char), size, fp);
	fclose(fp);
	return a;
}

void outfilechar(char* file, char* pbuf, DWORD size)
{
	FILE *fp;
	fp = fopen(file, "wb");
	if (!fp)
	{
		printf("文件打开错误 请检查文件地址！");
	}
	fwrite(pbuf, sizeof(char), size, fp);
	fclose(fp);
}

void outfilelong(char* file, long* pbuf, DWORD size)
{
	FILE *fp;
	fp = fopen(file, "wb");	
	if (!fp)
	{
		printf("文件打开错误 请检查文件地址！");
	}
	fwrite(pbuf, sizeof(long), size, fp);
	fclose(fp);
}

void outfiledouble(char * file, double * pbuf, DWORD size)
{
	FILE *fp;
	fp = fopen(file, "wb");
	if (!fp)
	{
		printf("文件打开错误 请检查文件地址！");
	}
	fwrite(pbuf, sizeof(double), size, fp);
	fclose(fp);
}
