#pragma once
#include "type.h"
#include "SgnlPrcsDll.h"

class  CSpectrumProbe
{
public:
	CSpectrumProbe(void);
	~CSpectrumProbe(void);

	DWORD InitSpectrumProbeParam(
		DWORD dwFFTSize, 
		DWORD dwFFTMove, 
		SHORT nAvgNum, 
		SHORT nWinType,
		bool  bIsComplex,
		bool  bInverse,
		bool bIslog);

	DWORD InputData(LONG* lpInData, DWORD dwInlen);

	DWORD InputData(double* lpInData, DWORD dwInlen);

	double* GetSpectrum(DWORD& dwLen);

protected:

	VOID ComputeWindow();

protected:
	// 数据通道频谱探测参数
	SPECTRUM_PROBE_PARAM m_SpecPara;

	bool m_bIsComplex;
	bool m_bIsInverse;
	bool m_bIslog;
	
	double* m_pAvgSpec;
	DWORD   m_dwSpecLen;

	// 输入信号
	double* m_pDataBuff;
	DWORD	m_dwDataLen;
	DWORD   m_dwInputLen;

	fftw_complex  *m_pFFTin;
	fftw_complex  *m_pFFTout;

	fftw_plan m_pFFT;

	SHORT m_nFFTWin;

	double *m_pWindow;

};
