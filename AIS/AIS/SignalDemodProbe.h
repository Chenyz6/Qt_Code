//////////////////////////////////////////////////////////////////////////
//
// 
// 
////////////////////////////////////////////////////////////////////////// 


#pragma once

#ifndef _SIGNAL_DEMOD_PROBE_H_
#define _SIGNAL_DEMOD_PROBE_H_
#include "SgnlPrcsDll.h"
#include "DataFIRDF.h"
#include <string.h>
#include "type.h"
typedef char CHAR;
typedef long LONG;
#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif


class CSignalDemodProbe
{
public:
	CSignalDemodProbe(void);

	~CSignalDemodProbe(void);

	DWORD InitDemodParam(double dSamplingFreq,double dSymbolRate, double dFreqShift,SHORT nModuType,bool bIsDecode);

	DWORD InputData(double *pData,DWORD dwDataLen);

    double* GetSignalDemod(DWORD& dwSignLen);

	long* GetSignalDemod(DWORD& dwSignLen,bool flag);

	CHAR* GetSignalSymbol(DWORD& dwBitNum);

protected:

	DWORD Demod_AM(double *pData,DWORD dwDataLen);

	DWORD Demod_FM(double *pData,DWORD dwDataLen);

	DWORD Demod_SSB(double *pData,DWORD dwDataLen);

	DWORD Demod_ASK(double *pData,DWORD dwDataLen);

	DWORD Demod_FSK(double *pData,DWORD dwDataLen);

	DWORD Demod_MSK(double *pData,DWORD dwDataLen);

	DWORD Demod_BPSK(double *pData,DWORD dwDataLen);

	DWORD Demod_QPSK(double *pData,DWORD dwDataLen);

	DWORD Demod_8PSK(double *pData,DWORD dwDataLen);

	DWORD Demod_16PSK(double *pData,DWORD dwDataLen);

	DWORD Demod_OQPSK(double *pData,DWORD dwDataLen);

	DWORD Demod_PI4DQPSK(double *pData,DWORD dwDataLen);

	DWORD Demod_SDPSK(double *pData,DWORD dwDataLen);	

	DWORD Demod_16QAM(double *pData,DWORD dwDataLen);

	DWORD Demod_32QAM(double *pData,DWORD dwDataLen);

    DWORD Demod_64QAM(double *pData,DWORD dwDataLen);

    DWORD Demod_256QAM(double *pData,DWORD dwDataLen);

    DWORD Decode_256QAM(LONG *pData,DWORD dwDataLen);
protected:

	DWORD InitParam_AMS();

	DWORD InitParam_PSK();

	DWORD InitParam_FSK();
	//获取解调参数
	void GetDemodPara(double *pCoef,BOOL bIsCL);
	//获取位同步参数
	void GetSymbolLoop(double *pCoef,SHORT M);
	//获取载波同步参数
	void GetCarrierLoop(double *pCoef);
	//获取AGC参数
	void GetAGC();

protected:
	//载波同步环路参数
	typedef struct{

		COMPLEX_SIGNAL dcDelay[2];

		double dDelay;

		double	dNCO_e;
		double	dNCO_p;

		double	dLpf_c1;
		double	dLpf_c2;
		double	dLpf_cc;

	}CARRIER_LOOP;

	//位同步环路参数
	typedef struct{

		SHORT	nIndex;

		COMPLEX_SIGNAL dcDelay;
		COMPLEX_SIGNAL dcMid;
	
		//内插抽取滤波器
		SHORT	nIntrFIR_bLen;
		SHORT	nIntrFIR_sLen;
		double	*pIntrFIR_b;

		COMPLEX_SIGNAL *pIntrFIR_s;

		//匹配滤波器
		SHORT	nMatchFIR_Len;
		double	*pMatcFIR_b;

		COMPLEX_SIGNAL *pMatcFIR_s;

		//DCO
		double	dDCO_f;
		double	dDCO_p;
		double	dDCO_e;

		//
		double	dLpf_c1;
		double	dLpf_c2;
		double	dLpf_cc;

	}SYMBOL_LOOP;

	//AGC参数
	typedef struct{

		double	a;
		double	s;
		double	m;

		SHORT	n;

		SHORT	nFIRLen;
		double	*pFIR_d;
	}AGC;

protected:
	double m_dSamplingFreq;

	double m_dSymbolRate;

	double m_dFreqShift;

	SHORT m_nModuType;	

protected:
	LONG* m_pDemodBuf;
	short* m_spSgnlBuf;
	DWORD	m_dwDemLen;
	// 输出符号个数
	DWORD	m_dwSylNum;

protected:
	CARRIER_LOOP m_Clpf;

	SYMBOL_LOOP  m_Tlpf;

	AGC  m_Agc;

private:
	CDataFIRDF* m_pFilter;

	COMPLEX_SIGNAL m_dcPreData;

	double* m_pFSKDelay;
	SHORT	m_nFSKDelay;

	double* m_pSgnlBuf;

	DWORD   m_dwSgnlLen;

protected:

	// 差分解码
	bool m_bIsDecode;

	// 比特流
	CHAR* m_pBitBuf;	

	DWORD m_dwBufLen;
	// 比特流个数
	DWORD m_dwBitNum;

	CHAR m_PreCode[4];

protected:
	DWORD InputSignal(LONG *pData,DWORD dwDataLen);	

	DWORD Decode_2Lvl(LONG *pData,DWORD dwDataLen);

	DWORD Decode_4Lvl(LONG *pData,DWORD dwDataLen);

	DWORD Decode_4Phase(LONG *pData,DWORD dwDataLen);

	DWORD Decode_8PSK(LONG *pData,DWORD dwDataLen);

	DWORD Decode_16PSK(LONG *pData,DWORD dwDataLen);

	DWORD Decode_16APSK(LONG *pData,DWORD dwDataLen);

	DWORD Decode_16QAM(LONG *pData,DWORD dwDataLen);

	DWORD Decode_32QAM(LONG *pData,DWORD dwDataLen);

	DWORD Decode_64QAM(LONG *pData,DWORD dwDataLen);
};


#endif //_SIGNAL_DEMOD_PROBE_H_
