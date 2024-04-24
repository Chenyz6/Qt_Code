#pragma once
#ifndef LDPC_ENCODE
#define LDPC_ENCODE
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <iterator>
#include <numeric>

using namespace std;
/***************基于高斯消元的LDPC编码器****************/
class ldpcencoder
{
public:
	ldpcencoder() = default;
	void GenerateHMatrix(int c, int r);
	void PrintfMatrix() const;
	void PrintfGuassMatrix() const;
	void TransHMatrix();
	void StartEncode(const vector<int> &signal);
	vector<int> GetEncodeResult() const;
	void PrintResult() const;
	vector<int> encoderesult;
	void Clear();
private:
	vector<vector<int>> HMatrix;
	vector<vector<int>> GuassHMatrix;
	
	int row;
	int column;
};

#endif
