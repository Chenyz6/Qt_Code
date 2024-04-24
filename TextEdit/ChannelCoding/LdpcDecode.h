#pragma once
#ifndef LDPC_DECODER
#define LDPC_DECODER

#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <cmath>
#include <sstream>
#include <string>
#include <math.h>

using namespace std;

/************���������ʹ�õ���LLR-BP������*************/

/****************������Ϣ�ڵ���У��ڵ�*****************/
class NodeData {
public:
	class parentnode
	{
	public:
		parentnode(int maxc, int length);
		~parentnode() { };
		int code_length;
		int size;//����Ϊ1����
		vector<int>cindex;//�洢1λ�õ�����
		vector<int>cmessage;//�洢�нڵ���Ϣ
	};
	class childnode
	{
	public:
		childnode(int maxr, int length);
		~childnode() {};
		int code_length;
		int size;//����Ϊ1����
		vector<int>rindex;//�洢1λ�õ�����
		vector<int>rmessage;//�洢�нڵ���Ϣ
	};
};
/***************����LLR-BP��LDPC��������****************/
class ldpcdecoder :public NodeData
{
public:
	ldpcdecoder() = default;  //Ĭ�Ϲ��캯��
	void IitialLdpcDecoder(int length); //��ʼ��������
	void GetHMatrix(int nn, int rr);//��ȡH�����ܶ���Ϣ 
	void SetMaxIter(int iter);//�趨����������
	vector<int> StartDecode(vector<double>channel_out, const double n0);
	
	//��ʼ��������
	int GetTheIter() const; //���������������
	void PrintResult() const; //��ʾ��������Ľ��
	void Clear();//�������
	~ldpcdecoder() {}; //��������������
	vector<int> result; //�洢����֮��Ľ��
    vector<double> LLR;//�ŵ������Ȼ�ȵ�ֵ
//private:
	parentnode CreateParentNode(int cw);//������Ϣ�ڵ�
	childnode CreateChildNode(int rw);
	vector<parentnode> message_node;//��Ϣ�ڵ�
	vector<childnode>  check_node; //У��ڵ�
	int NN;		 //��Ϣλ����
	int RR;		 //У��λ����
	int code_length;//��ĳ���
	int max_iter; //������������Ĭ��Ϊ50�Σ�
	int iter;//ʵ�ʵ�������
};


#endif


