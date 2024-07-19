#pragma once

// #include<afxwin.h>
#include "SgnlPrcsDll.h"
#include "DataFIRDF.h"
#include"type.h"

class CDataChannelize
{
public:
	CDataChannelize(void);
	~CDataChannelize(void);

	//信道化参数初始化
	DWORD InitChannelizeParam(
		double dSamplingFreq,
		double dSingalFreq,
		double dPassWidth,
		double dStopWidth,
		double &dOutSamplingFreq,
		short* pFactor = NULL,
		short  nFIRnum = 0);

	DWORD InputData(LONG* lpData, DWORD dwlen, bool bIsComplex);

	double* GetSignal(DWORD& dwDataLen);

protected:

	bool GetResamplingFactor(double dSamplingFreq, double dPassWidth, double &dOutSamplingFreq);

private:

	double m_dSamplingFreq;

	SIGNAL_CHANNEL_PARAM m_SignChanPara;	
	
	//输出缓存区
	double* m_pOutBuf;
	//输出缓存区长度
	DWORD  m_dwOutLen;	
	
	//信道化后的I路数据
	double *m_pSignI;
	//信道化后的Q路数据
	double *m_pSignQ;
	//信道化后的IQ长度
	DWORD	m_dwSignLen;

	//信道化后输出信号长度
	DWORD m_dwIQOut;

	//DDC相位
	double m_dCurrentPhase;	

	

	// 内插  interpolation //
	bool m_bInterpolated;
	//滤波内插倍数
	short		*m_pInterFactor;
	//滤波器组长度
	short		m_nInterFIRnum;
	//信号滤波器组
	CDataFIRDF **m_pInterFilter;	

	// 抽取 decimation //
	bool m_bDecimated;
	// 滤波抽取倍数
	short		*m_pDecimFactor;
	// 抽取序号
	DWORD		*m_pDecimIndex;	
	// 信号滤波器组
	CDataFIRDF **m_pDecimFilter;	
	// 滤波器组长度
	short		m_nDecimFIRnum;

};
