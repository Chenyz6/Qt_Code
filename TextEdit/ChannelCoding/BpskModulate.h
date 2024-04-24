#pragma once
#ifndef BPSK_MODULATE
#define BPSK_MODULATE
#include <vector>

using namespace std;
/*****BPSKµ÷ÖÆÆ÷******/
class bpskmodulator
{
public:
	bpskmodulator() = default;
	bpskmodulator(const vector<int> &signal);
	void GenerateBpsk(const vector<int> &signal);
	vector<double> GetBpskSignal() const;
	void PrintResult() const;
	void Clear();
private:
	vector<double> bpsksignal;
	void bpsk(const vector<int> &signal);
	int length;
};


#endif