// SgnlPrcsDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include"SgnlPrcsDll.h"


////////////////////////// 信号处理模块加载参数 ////////////////////////

SHORT g_SysModuType[SMT_NUM_MAX] = 
{
	SMT_UNKNOWN,
	SMT_NOISE, 
	SMT_CW, 
	SMT_AM, 
	SMT_FM,
	SMT_DSB,
	SMT_SSB,	
	SMT_MSK,
	SMT_GMSK,	
	SMT_2FSK,
	SMT_4FSK,
	SMT_2ASK,
	SMT_4ASK,
	SMT_2CPM,
	SMT_4CPM,
	SMT_BPSK,
	SMT_QPSK,
	SMT_8PSK,
	SMT_OQPSK,
	SMT_SDPSK,
	SMT_PI4DQPSK,
	SMT_8QAM,
	SMT_16QAM,
	SMT_32QAM,
	SMT_64QAM,	
	SMT_16PSK,	
	SMT_16APSK,
	SMT_USB_LINK11,
	SMT_USB_LINK11,
	SMT_ISB_LINK11,
	SMT_UHF_LINK11,
	SMT_UHF_LINK4A	
};

// TCHAR* g_SysModuName[SMT_NUM_MAX] =
// {
// 	TEXT("UNKNOWN"),
// 	TEXT("NOISE"),
// 	TEXT("CW"),
// 	TEXT("AM"),
// 	TEXT("FM"),
// 	TEXT("DSB"),
// 	TEXT("SSB"),
// 	TEXT("MSK"),
// 	TEXT("GMSK"),
// 	TEXT("2FSK"),
// 	TEXT("4FSK"),
// 	TEXT("2ASK"),
// 	TEXT("4ASK"),
// 	TEXT("2CPM"),
// 	TEXT("4CPM"),
// 	TEXT("BPSK"),
// 	TEXT("QPSK"),
// 	TEXT("8PSK"),
// 	TEXT("OQPSK"),
// 	TEXT("SDPSK"),
// 	TEXT("PI4DQPSK"),
// 	TEXT("8QAM"),
// 	TEXT("16QAM"),
// 	TEXT("32QAM"),
// 	TEXT("64QAM"),
// 	TEXT("16PSK"),
// 	TEXT("16APSK"),
// 	TEXT("USB_LINK11"),
// 	TEXT("LSB_LINK11"),
// 	TEXT("SSB_LINK11"),
// 	TEXT("UHF_LINK11"),
// 	TEXT("UHF_LINK4A")
// };

SHORT g_SysFHModuType[SMT_NUM_MAX] = 
{
	SMT_UNKNOWN,
	SMT_NOISE, 
	SMT_CW, 
	SMT_AM, 
	SMT_FM,
	SMT_DSB,
	SMT_SSB,	
	SMT_MSK,
	SMT_GMSK,	
	SMT_2FSK,
	SMT_4FSK,
	SMT_2ASK,
	SMT_4ASK,
	SMT_2CPM,
	SMT_4CPM,
	SMT_BPSK,
	SMT_QPSK,
	SMT_8PSK,
	SMT_OQPSK,
	SMT_SDPSK,
	SMT_PI4DQPSK,
	SMT_8QAM,
	SMT_16QAM,
	SMT_32QAM,
	SMT_64QAM,	
	SMT_16PSK,	
	SMT_16APSK,
	SMT_USB_LINK11,
	SMT_LSB_LINK11,
	SMT_ISB_LINK11,
	SMT_UHF_LINK11,
	SMT_UHF_LINK4A	
};


 TCHAR g_pzdllSetupFile[OSA_MAX_PATH];
 TCHAR g_pzdllTmpFilePath[OSA_MAX_PATH];

// TCHAR** CONST g_pdllSysModuName = g_SysModuName;

SHORT	 g_ndllModuTypeNum = SMT_NUM_MAX;
SHORT*  CONST g_pdllSysModuType = g_SysModuType;

SHORT	 g_ndllFHModuNum = SMT_NUM_MAX/2;
SHORT*  CONST g_pdllFHModuType = g_SysFHModuType;

// WORD	g_dllDataChanNum = 8;
// WORD	g_dllSignalChanNum = 8;

SHORT  g_dllLPFType = FIRLPFDT_Blackman;
double g_dllStopwidthFactor = 1.25; 
double g_dllPassRipple = 0.001;
double g_dllStopRipple = 60;

double g_dllDemodFactor = 1.35; 
double g_dllExtractMax = 1024.0; 

////////////////////////////////////////////////////////////////////////



//==========================================================================
// 求最大值 // 
LONG GetMaxVal(LONG* X,int N)
{
	LONG dMaxValue = X[0];
	for(int i=0;i<N;i++)
	{
		dMaxValue = MAX(X[i],dMaxValue);
	}
	return(dMaxValue);
}

double GetMaxVal(double* X,int N)
{
	double dMaxValue = X[0];
	for(int i=0;i<N;i++)
	{
		dMaxValue = MAX(X[i],dMaxValue);
	}
	return(dMaxValue);
}

LONG GetMaxIndex(double* X, LONG N, double& dMaxValue)
{
	LONG lMaxIndex = 0;

	dMaxValue = X[0];
	for(int i=0;i<N;i++)
	{
		if(X[i] > dMaxValue)
		{
			lMaxIndex = i;
			dMaxValue = X[i];
		}
	}
	return(lMaxIndex);
}

//==========================================================================
//  求最小值 //  
LONG GetMinVal(LONG* X,int N)
{
	LONG dMinValue = X[0];
	for(int i=0;i<N;i++)
	{
		dMinValue = MIN(X[i],dMinValue);		
	}
	return(dMinValue);
}

double GetMinVal(double* X,int N)
{
	double dMinValue = X[0];
	for(int i=0;i<N;i++)
	{
		dMinValue = MIN(X[i],dMinValue);
	}
	return(dMinValue);
}


//==========================================================================
// 信号归一化 // 
bool SignalNormalize(double* pData,long dwDataLen)
{
	DWORD i = 0;
	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}

	dSignNormal = sqrt(dSignNormal/dwDataLen);

	if (dSignNormal < DBL_EPSILON)
	{

		dSignNormal = 1.0;
	}

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] /= dSignNormal;
	}

	return(true);
}

extern  bool SignalNormalize(double* pData, unsigned long dwDataLen) {
    DWORD i = 0;
    ////计算信号规一化参数////
    double dSignNormal=0.0;
    for(i=0;i<dwDataLen/2;i++)
    {
        dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
    }

    dSignNormal = sqrt(dSignNormal/dwDataLen);

    if (dSignNormal < DBL_EPSILON)
    {

        dSignNormal = 1.0;
    }

    for(i=0;i<dwDataLen;i++)
    {
        pData[i] /= dSignNormal;
    }

    return(true);
}

bool SignalNormalize(LONG* pData,double* pSigIQ, DWORD dwDataLen)
{
	DWORD i = 0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}
	dSignNormal = sqrt(dSignNormal/dwDataLen);

	if (dSignNormal < DBL_EPSILON)
	{
		dSignNormal = 1.0;
	}

	for(i=0;i<dwDataLen;i++)
	{
		pSigIQ[i] = pData[i]/dSignNormal;
	}

	return(true);
}

//==========================================================================
// 计算信号的平方 //
void CalSignalSquare(double *pDataI, double *pDataQ, LONG lDataLen)
{
	double dDataI = 0.0;
	double dDataQ = 0.0;

	for(LONG i=0;i<lDataLen;i++)
	{
		dDataI = pDataI[i]*pDataI[i] - pDataQ[i]*pDataQ[i];
		dDataQ = 2*pDataI[i]*pDataQ[i];

		pDataI[i] = dDataI;
		pDataQ[i] = dDataQ;				
	}
}

void CalSignalSquare(double *pData, LONG nDataLen)
{
	double dDataI = 0.0;
	double dDataQ = 0.0;

	for(LONG i=0;i<nDataLen/2;i++)
	{
		dDataI = pData[2*i]*pData[2*i] - pData[2*i+1]*pData[2*i+1];
		dDataQ = 2*pData[2*i]*pData[2*i+1];

		pData[2*i] = dDataI;
		pData[2*i+1] = dDataQ;				
	}
}

//==========================================================================
// 信号瞬时包络 // 
void CalSingalEnvelope(double* pData,LONG nDataLen,double* pResult)
{
	for(LONG i=0;i<nDataLen/2;i++)
	{
		pResult[i] = sqrt(pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1]);		
	}
}
//==========================================================================
// 信号瞬时频率 // 
void CalSingalFrequency(double* pData,LONG nDataLen,double* pResult)
{
	double ti,tr;
	for(LONG i=0;i<nDataLen/2-1;i++)
	{
		ti = pData[2*i]*pData[2*i+3] - pData[2*i+2]*pData[2*i+1];
		tr = pData[2*i+2]*pData[2*i] + pData[2*i+3]*pData[2*i+1];

		pResult[i] = atan2(ti,tr);				
	}
	pResult[nDataLen/2-1]=0.0;	
}

//==========================================================================
// 信号瞬时相位 // 
void CalSingalPhase(double* pData,LONG nDataLen,double* pResult)
{
	for(LONG i=0;i<nDataLen/2;i++)
	{
		pResult[i] = atan2(pData[2*i+1],pData[2*i]);		
	}	
}

//==========================================================================
// 获取信号频率 // 
double GetSignalFreq(double dSignalFreq,DATA_CHANNEL_PARAM* pChanPara,bool bInv)
{
	double dTuneFreq = pChanPara->dTuneFreq; 
	double dIntermediateFreq = pChanPara->dIntermediateFreq;
	double dSamplingFreq = pChanPara->dSamplingFreq;
	bool  bInverse = pChanPara->bIsInverse && bInv;

	double dFreq = 0;

	if (pChanPara == NULL)
	{
        // jiaAssert(pChanPara != NULL);
		return dFreq;
	}

	// 调谐频率为零 //
	if (fabs(dTuneFreq) < DBL_EPSILON)
	{
		dTuneFreq = dIntermediateFreq;
	}

	if (pChanPara->bIsComplex)
	{
		dFreq = dTuneFreq + (bInverse ? -1.0 : 1.0)*dSignalFreq;
	}
	else
	{
		int n = static_cast<int>(2*dIntermediateFreq/dSamplingFreq);
		double dInterFreq = 0;

		if (n%2==0)
		{
			dInterFreq = dIntermediateFreq - n*dSamplingFreq/2;
		}
		else
		{
			dInterFreq = dSamplingFreq/2-(dIntermediateFreq - n*dSamplingFreq/2);
		}


		dFreq = dTuneFreq + (bInverse ? -1.0 : 1.0)*(dSignalFreq - dInterFreq);
	}	

	return dFreq;
}
//==========================================================================
// 获取数字频率 // 
double GetDigitalFreq(double dSignalFreq,DATA_CHANNEL_PARAM* pChanPara,bool bInv)
{
	double dTuneFreq = pChanPara->dTuneFreq; 
	double dIntermediateFreq = pChanPara->dIntermediateFreq;
	double dSamplingFreq = pChanPara->dSamplingFreq;
	bool  bInverse = pChanPara->bIsInverse && bInv;

	double dFreq = 0;

	if (pChanPara == NULL)
	{
        // jiaAssert(pChanPara != NULL);
		return dFreq;
	}

	// 调谐频率为零 //
	if (fabs(dTuneFreq) < DBL_EPSILON)
	{
		dTuneFreq = dIntermediateFreq;		
	}

	if (pChanPara->bIsComplex)
	{
		dFreq = (bInverse ? -1.0 : 1.0)*(dSignalFreq - dTuneFreq);//
	}
	else
	{
		int n = static_cast<int>(2*dIntermediateFreq/dSamplingFreq);
		double dInterFreq = 0;

		if (n%2==0)
		{
			dInterFreq = dIntermediateFreq - n*dSamplingFreq/2;
		}
		else
		{
			dInterFreq = dSamplingFreq/2-(dIntermediateFreq - n*dSamplingFreq/2);
		}


		dFreq = (bInverse ? -1.0 : 1.0)*(dSignalFreq - dTuneFreq) +  dInterFreq;
	}		

	return dFreq;
}


