#pragma once
#ifndef __BASICFUNC__
#define __BASICFUNC__
// #include"stdafx.h"
#include<iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include "type.h"
// #include <atlstr.h>
using namespace std;

// #define UNICODE
// #ifndef UNICODE
//     #define __T(x)      L ## x
//     #define _T(x)       __T(x)
//     #define _TEXT(x)    __T(x)
// #else
//     #define _T(x)       x
// #endif /* UNICODE */

#define _ttoi(x) atoi(x)




typedef struct rec
{
	std::string field;
	std::string val;
}Record;

typedef struct sr
{
	int matchIndex;
	int type;
	bool flag_bit_not;
	int len;
}SearchRet;

std::string deScramble(std::string data,vector<int> regIni, vector<int> poly);//普通解扰
std::string deScramble9600(std::string bin);
std::string deStuffing9600(std::string bin);
double biterr(std::string b1,std::string b2);//计算误码率
/////////////////////////////函数原型/////////////////////////////////////////////
std::string bitNot(std::string bin);//数据取反
std::string fliplr(std::string str);//高低位调换
std::string byteReverse(std::string bin);
int binstr2decnum(std::string str, std::string msbLoc="left-msb");
std::string binstr2decstr(std::string str, std::string msbLoc="left-msb");
std::string decnum2binstr(int d,std::string msbLoc="left-msb",int size=8);
std::string binstr2hexstr(std::string str, std::string msbLoc="left-msb");
std::string binstr2octstr(std::string str, std::string msbLoc="left-msb");
std::string binstr2ascii(std::string bin, int asciiSize,std::string msbLoc="left-msb");
void str2intvec(std::string input,vector<int>& intvec);
int* binstr2binvec(std::string input);
std::string binvec2binstr(int* input, int len);
std::string hex2bin(std::string str);

std::string  Xor(std::string bin1,std::string bin2);
std::string And(std::string  bin1 , std::string bin2);
int sum(std::string bin);


std::string deInterleave1(std::string bin,int r,int l);//解交织
std::string deInterleave2(std::string bin,int r,int l);//解交织
std::string phaseDebluer(std::string bin,std::string type);
std::string diffDecode2(std::string code);//解2相差分
std::string diffDecode4(std::string code);//解4相差分
std::string diffDecode8(std::string code);//解8相差分
std::string fskExtend_csc(std::string code);//fsk扩展
std::string fskExtend(std::string code);//fsk扩展
std::string qpskExtend(std::string code);//qpsk扩展
std::string oqpskExtend(std::string code);//oqpsk扩展
std::string oddEvenCheck(std::string msg,std::string type);//奇偶校验
std::string preProcessing(std::string bin,vector<int> flag);
void splitstring(std::string str,std::string mark,vector<std::string>& strVec);

#endif
