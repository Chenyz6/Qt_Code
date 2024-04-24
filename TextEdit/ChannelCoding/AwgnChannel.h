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

/******高斯白噪声信道产生器******/
using namespace std;

static default_random_engine e_channel(static_cast<unsigned>(time(NULL)));//利用static和时间避免每次运行出现相同的随机数

class awgnchannel
{
public:
	awgnchannel() = delete;    //禁止编译器进行默认构造函数
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
