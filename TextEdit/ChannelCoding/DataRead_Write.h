#pragma once
typedef  unsigned long DWORD;
/*	dat文件读写	 */

// 返回数据长度.dat
DWORD GetFilelen(const char* file,int s_len);
// 读short类型文件.dat
short* GetFileshort(const char* file, DWORD size);
// 读char类型文件.dat
char* GetFilechar(char* file, DWORD size);

// 写char类型文件.dat
void outfilechar(const char* file, char* pbuf, DWORD size);
// 写long类型文件.dat
void outfilelong(char* file, long* pbuf, DWORD size);
// 写double类型文件
void outfiledouble(char* file, double* pbuf, DWORD size);
