#pragma once
#ifndef GUASS_H
#define GUASS_H 

#include <random>
#include <vector>
#include <memory>
#include <iostream>
#include <ctime>
#include <cmath>
#include <stdio.h>
#include <math.h>

/******��˹�������ŵ�������******/
using namespace std;

static default_random_engine e_channel(static_cast<unsigned>(time(NULL)));//����static��ʱ�����ÿ�����г�����ͬ�������

class awgnchannel
{
public:
	awgnchannel() = delete;    //��ֹ����������Ĭ�Ϲ��캯��
	awgnchannel(double SNR, double mean, const vector<double> &signal);
	void SetEs(double E);
	void SetRate(double rate);
	vector<double> GetSignalWithNoise() const;
	vector<double> GetNoiseVector() const;
	void PrintResult() const;
	double GetN0() const;
	void Clear();
private:
	void AddNoise(const vector<double> &signal);
	double Es;
	double m;
	double sigma;
	double snr;
	double N0;
	double Rate;
	vector<double> SignalWithNoise;
	vector<double> Noise;
	int length;
};

#endif 
