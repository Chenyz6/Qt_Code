#pragma once
#include"type.h"
// typedef  unsigned long DWORD;
/*	dat文件读写	 */

// 返回数据长度.dat
DWORD GetFilelen(char* file,int s_len);
// 读short类型文件.dat
short* GetFileshort(char* file, DWORD size);
// 读char类型文件.dat
unsigned char* GetFilechar(char* file, DWORD size);

// 写char类型文件.dat
void outfilechar(char* file, char* pbuf, DWORD size);
// 写long类型文件.dat
void outfilelong(char* file, long* pbuf, DWORD size);
// 写double类型文件.dat
void outfiledouble(char* file, double* pbuf, DWORD size);

void outfileshort(char* file, short* pbuf, DWORD size);

