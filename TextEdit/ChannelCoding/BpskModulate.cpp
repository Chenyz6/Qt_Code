#include "BpskModulate.h"
#include <iostream>
/************************���캯��***********************/
bpskmodulator::bpskmodulator(const vector<int> &signal)
{
	bpsksignal.reserve(signal.size());
	for (size_t i = 0; i != signal.size(); ++i)
		bpsksignal.push_back(0.0);
	bpsk(signal);
}
/************************�����ź�***********************/
void bpskmodulator::GenerateBpsk(const vector<int> &signal)
{
	bpsk(signal);
}
/************************�����ź�***********************/
void bpskmodulator::bpsk(const vector<int> &signal)
{
	length = signal.size();
	for (size_t i = 0; i != length; ++i)
	{
		if (signal[i] == 1)
			bpsksignal[i] = -1;
		else
			bpsksignal[i] = 1;
	}
}
/************************����ź�***********************/
vector<double> bpskmodulator::GetBpskSignal() const
{
	return bpsksignal;
}
/************************��ӡ�ź�***********************/
void bpskmodulator::PrintResult() const
{
	for (size_t i = 0; i != length; ++i)
		cout << bpsksignal[i] << ends;
}
/************************�������***********************/
void bpskmodulator::Clear()
{
	bpsksignal.clear();
}