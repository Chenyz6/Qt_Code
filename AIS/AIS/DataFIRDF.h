#pragma once
#include "string.h"
#include "SgnlPrcsDll.h"
// typedef char CHAR;
// typedef short SHORT;
// typedef long LONG;

class CDataFIRDF
{
public:
	CDataFIRDF(void);
	~CDataFIRDF(void);

	// 低通滤波器初始化
	int IinitialLPFPara(
		short nLPFDesiType,
		double dPassWidth,		// 通带
		double dStopWidth,		// <0.5
		double dPassRipple,    // 0.01
		double dStopAttenuation); // dB		阻带衰减

	// 数据滤波
	void Filter(double *pDataI,double *pDataQ,int nDataLen);

	// 数据滤波
	void Filter(double *pData,int nDataLen);

protected:
	//滤波器长度
	short  m_nLPFLen;
	//数据缓存器
	double	*m_pDataI;
	double	*m_pDataQ;
	//滤波器系数
	double	*m_pLPFBuf;

	//获取滤波器长度
	short LPFLength(
		short nWinType,
		double dPassWidth,
		double dStopWidth,
		double dPassRipple,
		double dStopAttenuation);

	//基于窗函数的滤波器设计
	void WindowFuncFD(short nWinType,
		double dBandidth,
		double dStopAttenuation);

	//等波纹滤波器设计
	void EquirippleFD(
		double dPassWidth,
		double dStopWidth,
		double dPassRipple,
		double dStopAttenuation);
};
