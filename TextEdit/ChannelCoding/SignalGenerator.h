#pragma once
#ifndef SIGNAL_GENERATOR_H
#define SIGNAL_GENERATOR_H

#include <random>
#include <vector>
#include <fstream>
#include <ctime>

/*****************ÐÅºÅÉú³ÉÆ÷*****************/
using namespace std;

static default_random_engine e_rsignal(static_cast<unsigned>(time(NULL)));
class signalgenerator
{
public:
	signalgenerator() = default;
	void GenerateRandomeSignal(int n);
	void Generate_1_Signal(int n);
	void Generate_0_Signal(int n);
	void PrintResult() const;
	vector<int> GetSignal() const;
	void Clear();
	~signalgenerator() { };
	vector<int> signal;
private:
	
	int length;

};


#endif