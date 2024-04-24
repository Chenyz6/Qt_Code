#include "AwgnChannel.h"
/****************���캯��*****************/
awgnchannel::awgnchannel(double SNR, double mean, const vector<double> &signal) :
	snr(SNR), m(mean)
{
	length = signal.size();
	SignalWithNoise.reserve(length);
	Noise.reserve(length);
	Es = 1.0;
	Rate = 0.5;
	N0 = Es / (pow(10.0, (snr / 10))*Rate);
	sigma = sqrt(N0 / 2);
	AddNoise(signal);
}
/****************�����źŵ�ǿ��ֵ��Ĭ��ֵΪ1.0��*****************/
void awgnchannel::SetEs(double E)
{
	Es = E;
}
/******************����������ʣ�Ĭ��Ϊ0.5��*****************/
void awgnchannel::SetRate(double rate)
{
	Rate = rate;
}
/***********************��ȡ�����ź�************************/
vector<double> awgnchannel::GetSignalWithNoise() const
{
	return SignalWithNoise;
}
/***********************��ȡ�����ź�************************/
vector<double> awgnchannel::GetNoiseVector() const
{
	return Noise;
}
/***********************��ȡ�ŵ�N0************************/
double awgnchannel::GetN0() const
{
	return N0;
}
/***********************������************************/
void awgnchannel::AddNoise(const vector<double> &signal)
{
	normal_distribution<double> nd(m, sigma);
	for (size_t i = 0; i != length; ++i)
		Noise.push_back(nd(e_channel));
	for (size_t i = 0; i != length; ++i)
		SignalWithNoise.push_back(signal[i] + Noise[i]);
}
/***********************��ӡ�����ź�************************/
void awgnchannel::PrintResult() const
{
	for (size_t i = 0; i != length; ++i)
		cout << SignalWithNoise[i] << ends;
}

/***********************�������************************/
void awgnchannel::Clear()
{
	SignalWithNoise.clear();
	Noise.clear();
}
