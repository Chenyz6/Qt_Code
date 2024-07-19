#ifndef __PROTOCOLUNPACKAGE__
#define __PROTOCOLUNPACKAGE__
#include "basicFunc.h"
#include <vector>
using namespace std;
class ProtocolUnpackage
{
public:
	void virtual initial(){}
	bool virtual search(){return true;}
	bool virtual unpackage(){return true;}
	void virtual processing(vector<int> flag){}
public:
	string bin;
	SearchRet searchRet;
	vector<Record> allInfo;

};

#endif