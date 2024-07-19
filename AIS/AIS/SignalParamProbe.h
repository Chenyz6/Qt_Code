////////////////////////////////////////////////////////////////////////// 
//
// 
// 
////////////////////////////////////////////////////////////////////////// 

#pragma once

#ifndef _SIGNAL_PARAM_PROBE_H_
#define _SIGNAL_PARAM_PROBE_H_
#include "SpectrumProbe.h"
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CSignalParamProbe
{
public:
	CSignalParamProbe(void);

	~CSignalParamProbe(void);

	// 初始化参数探测器
	DWORD InitParamProbe(double dSamplingFreq, double dPasswidth,DWORD dwFFTSize, SHORT nModuType = SMT_UNKNOWN);

	// 信号样式识别和参数估计
	DWORD InputData(double *pData, DWORD dwDataLen);

	// 获取信号参数
	DWORD GetSignalParam(SIGNAL_DEMOD_PARAM& pSignalPara);

protected:
	// Modulation Recognition
	SHORT ModuRecognition(double *pData, DWORD dwDataLen);

	// Parameter Estimate
	DWORD ParamEstimate(double *pData, DWORD dwDataLen);

	// Spectrum Analysis Parameters
	DWORD GetSpecAnalParam(double *pData, DWORD dwDataLen);

	double GetAmplAveSqu(double* pSignAmpl, DWORD dwDataLen);

	double GetFreqAveSqu(double* pSignFreq, DWORD dwDataLen);

	// 计算平均功率谱 //
	DWORD CalAverPowerSpecrum(double* pPoweSpec,double* pData, DWORD dwDataLen);
	// 计算平均幅度谱 //
	void CalAmplAverPowerSpecrum(double* pPoweSpec,double *pData, DWORD dwDataLen);
	// 初步估计信号带宽和中心频率 //
	void RoughEstiFcBnw(double& dCenterFreq, LONG& nBandwidth, double *pSignalPsd);

	void GetReferParam(double *pReferParam, double *pSignalPsd,LONG nBandwidth);

	void GetSpectrumParam(double *pSpecParam,double *pSignalPsd,LONG nBandwidth,double *pReferParam,short N);

	void GetSpecIdenParam(double* pIdenPara,double *pSpecParam2,double *pSpecParam4,double *pSpecParam8);

	void GetDiffSpecParam(double* pIdenPara,double* pSignAmpl,DWORD dwDataLen,LONG  nBandwidth,bool bIsFreq = FALSE);	

	void Amplitudenormalization(double* pFreqBuff,int N);

	void Frequencynormalization(double* pAmplBuff,int N,int M);	

	void NormalizationAmplFreq(double* pAmplBuff,double* pFreqBuff,int N,int M);

	// 提取瞬时包络/频率的统计特征参数
	double GetFrequencyStatParam(double* pInput,int Inlen);
	//零中心归一化瞬时幅度/频率的紧致性(四阶矩) // 
	double GetFourthOrderParam(double *x,int len);
	// 功率谱对称性参数 //
	double GetSpectraSymmetParam(double *pSignalPsd,DWORD dwPSDLen);
	// 获取功率谱参数 //
	void GetSpectraParam(double *pParam,double *pSignalPsd,DWORD dwPSDLen);
	// 获取6dB电平带宽//
	LONG GetLvlBandwidth(double *pSignalPsd,DWORD dwPSDLen, double dLvlThreshold);
	// 获取功率谱峰值数目//
	void GetSpectraPeaksNum(double *pParam,double *pSignalPsd,DWORD dwPSDLen);
	// 计算功率谱的中心频率 //
	double CalSpectraCenterFreq(double* pInput,int N);
	// 计算ASK的符号速率 //
	double CalASKSymbolRate(double* pDataIQ,DWORD dwDataLen);
	// 计算FSK的符号速率 //
	double CalFSKSymbolRate(double* pDataIQ,DWORD dwDataLen);
	// 利用功率谱计算FSk的中心频率 //
	void CalFSKCFDF_Spectra(double* pParam,double* pDataIQ,DWORD dwDataLen);	
	// 利用瞬时频率计算FSk的中心频率 //
	void CalFSKCFDF_InstFrequ(double* pParam,double* pDataIQ,DWORD dwDataLen);
	// 计算ASK的中心频率 //
	double CalASKFrequncy(double* pDataIQ,DWORD dwDataLen);
	// QAM信号识别 //
	SHORT QAMRecognition(double* pDataIQ,DWORD dwDataLen,double dSymbolLen);

private:

	CSpectrumProbe* m_pFFTIQ;
	CSpectrumProbe* m_pFFTAD;

	//输入采样频率 Hz
	double	m_dSamplingFreq;	
	// 输入通带带宽 Hz
	double m_dPasswidth;
	// 信号调制类型
	SHORT	m_nModuType;
	
	//估计信号频率 Hz
	double	m_dSignalFreq;	
	//信号调制速率 Hz
	double	m_dSymbolRate;
	//信号调制速率 Hz
	double	m_dFreqShift;

	// 功率谱分析的FFT点数
	DWORD	m_dwFFTSize;
	DWORD	m_dwFFTMove; 
	SHORT	m_nFFTAver;
	SHORT	m_nFFTWind;
	// 识别所需的信号数据长度
	DWORD	m_dwSignLen;

	DWORD  m_dwTmpBufLen;
	// 输入信号长度
	DWORD	m_dwIQLen;
	
	// 谱特征参数 Singal Stat. characteristic Parameters
	struct SpectrumAnanlysisParam{

		LONG   nBandwidth;	 // Estimate bandwidth
		double dCenterFreq;	 // Estimate center frequency

		double dAmplAveSqu;  // amplitude average square/amplitude square average
		double dDASPeakLvl;	 // Difference Amplitude Spectrum max.peak relative level
		LONG   nDASPeakIdx;  // Difference Amplitude Spectrum max.Peak Index	

		double dFreqAveSqu;  // amplitude average square/amplitude square average
		double dDFSPeakLvl;	 // Difference Amplitude Spectrum max.peak relative level
		LONG   nDFSPeakIdx;  // Difference Amplitude Spectrum max.Peak Index

		double  dAMForOrder;
		double  dFMForOrder;
		double  dFMStatWight;
		double  dAMStatWight;

		LONG   nSpecType;	 // Type of Spectrum
		LONG   nSpecPeakNum; // Spectrum peaks' number 
		LONG   nPeakIndex1;	 // 1st peak index
		LONG   nPeakIndex2;  // 2nd peak index
		double dSpecPeakLvl; // max.peak absolute level/max.peak relative level	

		LONG   nSquaPeakIdx1; // Square Spectrum max.Peak Index	
		LONG   nSquaPeakIdx2;

		LONG    nPsdPeakNum;  // Spectrum peaks' number	
		LONG    nPsdPeakIdx1; // 1st peak index
		LONG    nPsdPeakIdx2; // 2st peak index
		double  dPsdLvlBnd;	  // 功率谱电平带宽
		double  dPsdPeakDeT;  // 1st peak lvl - 2nd peak lvl
		double  dPsdCentFre;  // 功率谱对称中心频率
		double  dPsdSymmet;	  // 功率谱对称性
	}m_sap;

	double *m_pDataBuff;
	double *m_pSignalBuff;	
	double *m_pAmplBuff;	
	double *m_pFreqBuff;
	double *m_pTmpBufI;
	double *m_pTmpBufQ;

	double *m_pSignPsd;
	double *m_pAmplPSD;
	double *m_pBuffer1;
	double *m_pBuffer2;
	double *m_pBuffer3;
	LONG   *m_pPsdInex;
};


#endif // _SIGNAL_PARAM_PROBE_H_