#include "stdafx.h"
#include "SignalDemodFIR.h"
#include "SignalDemodProbe.h"



CSignalDemodProbe::CSignalDemodProbe(void)
{
	m_dSamplingFreq = 0.0;
	m_dSymbolRate   = 0.0;
	m_dFreqShift    = 0.0;
	m_nModuType     = SMT_UNKNOWN;

	m_pDemodBuf = NULL;
	m_dwDemLen = 0;
	m_dwSylNum = 0;

	m_Clpf.dcDelay[0].I = 0.0;
	m_Clpf.dcDelay[0].Q = 0.0;
	m_Clpf.dcDelay[1].I = 0.0;
	m_Clpf.dcDelay[1].Q = 0.0;
	m_Clpf.dDelay  = 0.0;
	m_Clpf.dLpf_c1 = 0.0;
	m_Clpf.dLpf_c2 = 0.0;
	m_Clpf.dLpf_cc = 0.0;
	m_Clpf.dNCO_e  = 0.0;
	m_Clpf.dNCO_p  = 0.0;

	m_Tlpf.dcDelay.I = 0.0;
	m_Tlpf.dcDelay.Q = 0.0;	
	m_Tlpf.dcMid.I = 0.0;
	m_Tlpf.dcMid.Q = 0.0;
	m_Tlpf.dDCO_e = 0.0;
	m_Tlpf.dDCO_f = 0.0;
	m_Tlpf.dDCO_p = 0.0;

	m_Tlpf.dLpf_c1 = 0.0;
	m_Tlpf.dLpf_c2 = 0.0;
	m_Tlpf.dLpf_cc = 0.0;

	m_Tlpf.nIndex = 0;
	m_Tlpf.nIntrFIR_bLen = 0;
	m_Tlpf.nIntrFIR_sLen = 0;
	m_Tlpf.nMatchFIR_Len = 0;

	m_Tlpf.pIntrFIR_b = NULL;
	m_Tlpf.pIntrFIR_s = NULL;
	m_Tlpf.pMatcFIR_b = NULL;
	m_Tlpf.pMatcFIR_s = NULL;

	m_Agc.a = 0.0;
	m_Agc.m = 0.0;
	m_Agc.s = 0.0;

	m_Agc.n = 0;
	m_Agc.nFIRLen = 0;	
	m_Agc.pFIR_d = NULL;

	m_pFilter = NULL;

	m_dcPreData.I = 0;
	m_dcPreData.Q = 0;

	m_pFSKDelay = NULL;
	m_nFSKDelay = 0;

	m_pSgnlBuf = NULL;
	m_dwSgnlLen = 0;

	///////////////////////////////////////////////////////////////////////
	m_pBitBuf = NULL;
	m_dwBufLen = 0;
	m_dwBitNum = 0;

	for (SHORT n = 0; n < 4; n++)
	{
		m_PreCode[n] = 0;
	}

	m_bIsDecode = false;
}

CSignalDemodProbe::~CSignalDemodProbe(void)
{
	if (m_pDemodBuf != NULL)
	{
		delete []m_pDemodBuf;
		m_pDemodBuf = NULL;
	}

	if (NULL != m_pFilter)
	{
		delete m_pFilter;
		m_pFilter = NULL;
	}

	if (NULL != m_Tlpf.pIntrFIR_b)
	{
		delete []m_Tlpf.pIntrFIR_b;
		delete []m_Tlpf.pIntrFIR_s;
		delete []m_Tlpf.pMatcFIR_b;
		delete []m_Tlpf.pMatcFIR_s;
	}

	if (NULL != m_Agc.pFIR_d)
	{
		delete []m_Agc.pFIR_d;
		m_Agc.pFIR_d = NULL;
	}

	if (NULL != m_pFSKDelay)
	{
		delete []m_pFSKDelay;
		m_pFSKDelay = NULL;
	}

	if (NULL != m_pSgnlBuf)
	{
		delete []m_pSgnlBuf;
		m_pSgnlBuf = NULL;
	}

	if (NULL != m_pBitBuf)
	{
		delete []m_pBitBuf;
		m_pBitBuf = NULL;
	}	
}

DWORD CSignalDemodProbe::InitDemodParam(double dSamplingFreq, double dSymbolRate, double dFreqShift, SHORT nModuType, bool bIsDecode)
{
	DWORD dwErrorID = ERROR_SUCCESS;

	m_dSamplingFreq = dSamplingFreq;
	m_dSymbolRate = dSymbolRate;
	m_dFreqShift  = dFreqShift;
	m_nModuType = nModuType;
	m_bIsDecode = bIsDecode;

	switch (m_nModuType)
	{
	case SMT_CW:
	case SMT_AM:
	case SMT_FM:
	case SMT_SSB:
	case SMT_DSB:
		dwErrorID = InitParam_AMS();
		break;
	case SMT_2ASK:
	case SMT_4ASK:
	case SMT_2FSK:
	case SMT_4FSK:
		dwErrorID = InitParam_FSK();
		break;
	case SMT_MSK:
	case SMT_GMSK:
	case SMT_BPSK:
	case SMT_QPSK:	
	case SMT_8PSK:
	case SMT_OQPSK:
	case SMT_SDPSK:
	case SMT_PI4DQPSK:		
	case SMT_16QAM:
	case SMT_32QAM:
	case SMT_64QAM:
	case SMT_16PSK:
	case SMT_16APSK:
	case SMT_8QAM:
		dwErrorID = InitParam_PSK();
		break;	
	default:
		dwErrorID = ERROR_UNDEFINED_MODULATION_TYPE;
		break;
	}

	return(dwErrorID);	
}

DWORD CSignalDemodProbe::InputData(double *pData, DWORD dwDataLen)
{
	DWORD dwErrorID = ERROR_SUCCESS;

	if (m_pDemodBuf != NULL && m_dwDemLen < dwDataLen )
	{
		delete []m_pDemodBuf;
		m_pDemodBuf = NULL;
	}	

	if (m_pDemodBuf == NULL)
	{
		m_dwDemLen = dwDataLen;
		m_pDemodBuf = new LONG[m_dwDemLen];
	}

	m_dwSylNum = 0;
	ZeroMemory(m_pDemodBuf,m_dwDemLen*sizeof(LONG));

	switch (m_nModuType)
	{	
	case SMT_AM:
		{
			dwErrorID = Demod_AM(pData,dwDataLen);
		}
		break;
	case SMT_FM:
		{
			dwErrorID = Demod_FM(pData,dwDataLen);
		}
		break;
	case SMT_CW:
	case SMT_SSB:
	case SMT_DSB:
		{
			dwErrorID = Demod_SSB(pData,dwDataLen);
		}
		break;
	case SMT_2ASK:
	case SMT_4ASK:
		{
			dwErrorID = Demod_ASK(pData,dwDataLen);
		}
		break;
	case SMT_2FSK:
	case SMT_4FSK:
		{
			dwErrorID = Demod_FSK(pData,dwDataLen);
		}
		break;		
	case SMT_MSK:
	case SMT_GMSK:
		{
			dwErrorID = Demod_MSK(pData,dwDataLen);
		}
		break;
	case SMT_BPSK:
		{
			dwErrorID = Demod_BPSK(pData,dwDataLen);
		}
		break;	
	case SMT_QPSK:
		{
			dwErrorID = Demod_QPSK(pData,dwDataLen);
		}
		break;	
	case SMT_8QAM:
	case SMT_8PSK:
		{
			dwErrorID = Demod_8PSK(pData,dwDataLen);
		}
		break;
	case SMT_16PSK:
		{
			dwErrorID = Demod_16PSK(pData,dwDataLen);
		}
		break;
	case SMT_OQPSK:
		{
			dwErrorID = Demod_OQPSK(pData,dwDataLen);
		}
		break;
	case SMT_PI4DQPSK:
		{
			dwErrorID = Demod_PI4DQPSK(pData,dwDataLen);
		}
		break;	
	case SMT_SDPSK:
		{
			dwErrorID = Demod_SDPSK(pData,dwDataLen);
		}
		break;
	case SMT_16QAM:
	case SMT_16APSK:
		{
			dwErrorID = Demod_16QAM(pData,dwDataLen);
		}
		break;	
	case SMT_32QAM:
		{
			dwErrorID = Demod_32QAM(pData,dwDataLen);
		}
		break;	
	case SMT_64QAM:
		{
			dwErrorID = Demod_64QAM(pData,dwDataLen);
		}
		break;	
	default:
		{
			dwErrorID = ERROR_UNDEFINED_MODULATION_TYPE;
		}
	}

	return(dwErrorID);
}


double* CSignalDemodProbe::GetSignalDemod(DWORD& dwSignLen)
{
	if (NULL == m_pDemodBuf || m_dwSylNum < 1)
	{
		return(NULL);
	}

	dwSignLen = m_dwSylNum;

	return(m_pSgnlBuf);
}

long * CSignalDemodProbe::GetSignalDemod(DWORD & dwSignLen, bool flag)
{

	if (NULL == m_pDemodBuf || m_dwSylNum < 1)
	{
		return(NULL);
	}

	dwSignLen = m_dwSylNum;

	return(m_pDemodBuf);
}

// 获取比特流 //
CHAR* CSignalDemodProbe::GetSignalSymbol(DWORD& dwBitNum)
{
	InputSignal(m_pDemodBuf, m_dwSylNum);

	if ( m_pBitBuf == NULL || m_dwBufLen == 0)
	{
		return(NULL);
	}	

	dwBitNum = m_dwBitNum;

	return(m_pBitBuf);
}

//////////////////////////////////////////////////////////////////////////

DWORD CSignalDemodProbe::InitParam_AMS()
{
	DWORD dwErrorID = ERROR_SUCCESS;

	if (NULL != m_pFilter)
	{
		delete m_pFilter;

		m_pFilter = NULL;
	}

	m_dcPreData.I = 0;
	m_dcPreData.Q = 0;

	m_pFilter = new CDataFIRDF;	

	short nLPFType = FIRLPFDT_Blackman;
	double dPasswidth = m_dSymbolRate/m_dSamplingFreq;
	double dStopwidth = g_dllStopwidthFactor*m_dSymbolRate/m_dSamplingFreq;
	double dPassRipple = 0.01;
	double dStopRipple = 60;

	if ( !m_pFilter->IinitialLPFPara(nLPFType, dPasswidth, dStopwidth, dPassRipple, dStopRipple) )
	{
		dwErrorID = ERROR_DEMOD_PARAM;
	}

	return(dwErrorID);
}

DWORD CSignalDemodProbe::InitParam_FSK()
{
	DWORD dwErrorID = ERROR_SUCCESS;

	if (NULL != m_pFilter)
	{
		delete m_pFilter;

		m_pFilter = NULL;
	}

	if (m_pFSKDelay != NULL)
	{
		delete []m_pFSKDelay;

		m_pFSKDelay = NULL;
	}

	if (NULL != m_Tlpf.pIntrFIR_b)
	{
		delete []m_Tlpf.pIntrFIR_b;
		delete []m_Tlpf.pIntrFIR_s;
		delete []m_Tlpf.pMatcFIR_b;
		delete []m_Tlpf.pMatcFIR_s;
	}

	if (NULL != m_Agc.pFIR_d)
	{
		delete []m_Agc.pFIR_d;
	}

	m_nFSKDelay = static_cast<SHORT>(m_dSamplingFreq/m_dSymbolRate);
	m_pFSKDelay = new double[m_nFSKDelay];
	ZeroMemory(m_pFSKDelay,m_nFSKDelay*sizeof(double));

	m_pFilter = new CDataFIRDF;

	short nLPFType = FIRLPFDT_Blackman;	
	double dPasswidth = 1.5*m_dSymbolRate/m_dSamplingFreq;
	double dStopwidth = g_dllStopwidthFactor*dPasswidth;
	double dPassRipple = 0.01;
	double dStopRipple = 60;

	if ( !m_pFilter->IinitialLPFPara(nLPFType, dPasswidth, dStopwidth, dPassRipple, dStopRipple) )
	{
		dwErrorID = ERROR_DEMOD_PARAM;
	}	

	// 初始化参数
	double pCoef[2] = {0,0};

	GetDemodPara(pCoef,FALSE);
	GetSymbolLoop(pCoef,64);

	GetAGC();

	return(dwErrorID);
}

DWORD CSignalDemodProbe::InitParam_PSK()
{
	DWORD dwErrorID = ERROR_SUCCESS;

	if (NULL != m_Tlpf.pIntrFIR_b)
	{
		delete []m_Tlpf.pIntrFIR_b;
		delete []m_Tlpf.pIntrFIR_s;
		delete []m_Tlpf.pMatcFIR_b;
		delete []m_Tlpf.pMatcFIR_s;
	}

	if (NULL != m_Agc.pFIR_d)
	{
		delete []m_Agc.pFIR_d;
	}

	// 初始化参数
	double pCoef[2] = {0,0};

	GetDemodPara(pCoef,FALSE);
	GetSymbolLoop(pCoef,64);

	GetDemodPara(pCoef,TRUE);
	GetCarrierLoop(pCoef);

	GetAGC();

	return(dwErrorID);
}

///获取解调参数///
void CSignalDemodProbe::GetDemodPara(double *pCoef,BOOL bIsCL)
{	
//	pCoef[0] = sqrt(2.0)/2;

	if(bIsCL)	//Carrier-Loop
	{
		switch(m_nModuType)
		{	
		case SMT_64QAM:
			pCoef[0] = sqrt(2.0)/2;
			pCoef[1] = 0.025/16; 
			break;
		case SMT_32QAM:
			pCoef[0] = sqrt(2.0)/2;
			pCoef[1] = 0.025/2; 
			break;	
		case SMT_16PSK:
		case SMT_16APSK:
		case SMT_16QAM:
			pCoef[0] = sqrt(2.0)/2;
			pCoef[1] = 0.025/2; 
			break;
		case SMT_8QAM:
		case SMT_8PSK:
		case SMT_PI4DQPSK:
			pCoef[0] = sqrt(2.0)/2;
			pCoef[1] = 0.025/4; 
			break;
		default:
			pCoef[0] = sqrt(2.0)/2;
			pCoef[1] = 0.025/2; 
			break;
		}
	}
	else		//Timing-Loop
	{
		switch(m_nModuType)
		{
		case SMT_64QAM:		
		case SMT_32QAM:
			pCoef[0] = sqrt(2.0)/4;
			pCoef[1] = 0.00625; 
			break;	
		case SMT_16PSK:
		case SMT_16APSK:
		case SMT_16QAM:
			pCoef[0] = sqrt(2.0)/4;
			pCoef[1] = 0.0125; 
			break;
		case SMT_8QAM:
		case SMT_8PSK:
		case SMT_PI4DQPSK:
			pCoef[0] = sqrt(2.0)/4;
			pCoef[1] = 0.0125/8;
			break;
		default:
			pCoef[0] = sqrt(2.0)/4;
			pCoef[1] = 0.0125/4; 
			break;
		}
	}
}

///获取位同步时钟参数///
void CSignalDemodProbe::GetSymbolLoop(double *pCoef, SHORT M)
{
	double dRatio,w,dTemp;

	dRatio = w = dTemp = 0.0;

	m_Tlpf.nIndex = 0;	

	m_Tlpf.dcDelay.I = 0.0;
	m_Tlpf.dcDelay.Q = 0.0;

	m_Tlpf.dcMid.I = 0.0;
	m_Tlpf.dcMid.Q = 0.0;	

	////内插抽取滤波器////
	m_Tlpf.nIntrFIR_bLen = IBL;
	m_Tlpf.pIntrFIR_b = new double[m_Tlpf.nIntrFIR_bLen];
	memcpy(m_Tlpf.pIntrFIR_b,IB,m_Tlpf.nIntrFIR_bLen*sizeof(double));

	m_Tlpf.nIntrFIR_sLen = IBL/M;	
	m_Tlpf.pIntrFIR_s = new COMPLEX_SIGNAL[m_Tlpf.nIntrFIR_sLen];
	memset(m_Tlpf.pIntrFIR_s,0,2*m_Tlpf.nIntrFIR_sLen*sizeof(double));

	////匹配滤波器////
	m_Tlpf.nMatchFIR_Len = MBL;
	m_Tlpf.pMatcFIR_b = new double[m_Tlpf.nMatchFIR_Len];
	memcpy(m_Tlpf.pMatcFIR_b,MB,m_Tlpf.nMatchFIR_Len*sizeof(double));

	m_Tlpf.pMatcFIR_s = new COMPLEX_SIGNAL[m_Tlpf.nMatchFIR_Len];
	memset(m_Tlpf.pMatcFIR_s,0,2*m_Tlpf.nMatchFIR_Len*sizeof(double));

	dRatio = m_dSymbolRate/m_dSamplingFreq;
	///位同步校正参数///
	m_Tlpf.dDCO_f = 2*dRatio;
	m_Tlpf.dDCO_e = 0.0;
	m_Tlpf.dDCO_p = 0.0;

	w = (2.0*PI)*pCoef[1]*dRatio*((1.0+4.0*pCoef[0]*pCoef[0])/(8.0*pCoef[0]));
	dTemp = 4.0+4.0*pCoef[0]*w+w*w;

	m_Tlpf.dLpf_c1 = 8.0*pCoef[0]*w/dTemp;
	m_Tlpf.dLpf_c2 = 4.0*w*w/dTemp;
	m_Tlpf.dLpf_cc = 0.0;
}

///获取载波环路参数///
void CSignalDemodProbe::GetCarrierLoop(double *pCoef)
{
	double w, dTemp;

	w = dTemp = 0.0;

	m_Clpf.dDelay = 0.0;

	m_Clpf.dNCO_e = 0.0;
	m_Clpf.dNCO_p = 0.0;

	m_Clpf.dcDelay[0].I=0.0;
	m_Clpf.dcDelay[0].Q=0.0;
	m_Clpf.dcDelay[1].I=0.0;
	m_Clpf.dcDelay[1].Q=0.0;

	w = (2.0*PI)*pCoef[1]*0.5*((1.0+4.0*pCoef[0]*pCoef[0])/(8.0*pCoef[0]));
	dTemp = 4.0+4.0*pCoef[0]*w+w*w;

	///载波校正参数///
	m_Clpf.dLpf_c1 = 8.0*pCoef[0]*w/dTemp;
	m_Clpf.dLpf_c2 = 4.0*w*w/dTemp;
	m_Clpf.dLpf_cc=0.0;
}

///获取AGC参数///
void CSignalDemodProbe::GetAGC()
{
	m_Agc.a = 0.01;
	m_Agc.s = 0.0;
	m_Agc.m = 1.0;
	m_Agc.n = 0;

	m_Agc.nFIRLen = 10;
	m_Agc.pFIR_d = new double[m_Agc.nFIRLen];
	memset(m_Agc.pFIR_d,0,m_Agc.nFIRLen*sizeof(double));
}

DWORD CSignalDemodProbe::Demod_AM(double *pData, DWORD dwDataLen)
{
	if (m_pSgnlBuf != NULL && m_dwSgnlLen < dwDataLen/2)
	{
		delete []m_pSgnlBuf;
		m_pSgnlBuf = NULL;
	}	

	if (m_pSgnlBuf == NULL)
	{
		m_dwSgnlLen = dwDataLen/2;
		m_pSgnlBuf = new double[m_dwSgnlLen];
	}

	DWORD i=0;
	double dSignNormal = 0.0;
	
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];		
	}
	
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON	;
	
	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}

	////求瞬时幅度////
	for(i = 0; i < dwDataLen/2; i++)	
	{
		m_pSgnlBuf[i] = sqrt(pData[2*i]*pData[2*i] + pData[2*i+1]*pData[2*i+1]) - 1.25;
	}
	m_pFilter->Filter(m_pSgnlBuf,dwDataLen/2);

	// 量化 //
	for(i=0;i<dwDataLen/2;i++)
	{
		m_pDemodBuf[i] = static_cast<LONG>(SHRT_MAX*(m_pSgnlBuf[i]));
	}

	m_dwSylNum = dwDataLen/2;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_FM(double *pData, DWORD dwDataLen)
{
	if (m_pSgnlBuf != NULL && m_dwSgnlLen < dwDataLen/2)
	{
		delete []m_pSgnlBuf;

		m_pSgnlBuf = NULL;
	}	

	if (m_pSgnlBuf == NULL)
	{
		m_dwSgnlLen = dwDataLen/2;

		m_pSgnlBuf = new double[m_dwSgnlLen];
	}

	DWORD i = 0;
	double dSignNormal = 0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];		
	}	
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	
	
	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}

	double tempI,tempQ;

	////求本次输入第一个点的瞬时频率////
	tempQ = pData[1]*m_dcPreData.I-pData[0]*m_dcPreData.Q;
	tempI = pData[0]*m_dcPreData.I+pData[1]*m_dcPreData.Q;

	m_pSgnlBuf[0] = atan2(tempQ,tempI);

	////求瞬时频率////
	for(i = 1; i < dwDataLen/2; i++)
	{
		tempQ = pData[2*i+1]*pData[2*i-2] - pData[2*i]*pData[2*i-1];
		tempI = pData[2*i]*pData[2*i-2] + pData[2*i+1]*pData[2*i-1];

		m_pSgnlBuf[i] = atan2(tempQ,tempI);
	}

	////保存本次输入的最后一个点////
	m_dcPreData.I = pData[dwDataLen-2];
	m_dcPreData.Q = pData[dwDataLen-1];

	m_pFilter->Filter(m_pSgnlBuf,dwDataLen/2);

	for(i=0;i<dwDataLen/2;i++)
	{
		m_pDemodBuf[i] = static_cast<LONG>(SHRT_MAX*m_pSgnlBuf[i]);
	}

	m_dwSylNum = dwDataLen/2;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_SSB(double *pData,DWORD dwDataLen)
{
	if (m_pSgnlBuf != NULL && m_dwSgnlLen < dwDataLen/2)
	{
		delete []m_pSgnlBuf;

		m_pSgnlBuf = NULL;
	}	

	if (m_pSgnlBuf == NULL)
	{
		m_dwSgnlLen = dwDataLen/2;

		m_pSgnlBuf = new double[m_dwSgnlLen];
	}

	DWORD i = 0;
	double dSignNormal = 0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];		
	}
	
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	
	
	for(i=0;i<dwDataLen/2;i++)
	{
		m_pSgnlBuf[i] = pData[2*i]/dSignNormal;
	}

	m_pFilter->Filter(m_pSgnlBuf,dwDataLen/2);	

	for(i=0;i<dwDataLen/2;i++)
	{
		m_pDemodBuf[i] = static_cast<LONG>(SHRT_MAX*m_pSgnlBuf[i]);
	}

	m_dwSylNum = dwDataLen/2;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_ASK(double *pData,DWORD dwDataLen)
{
	DWORD i, dwSignLen, dwCounter;
	INT k,nClock,nFIRIndex,M;
	double dAmplit, dPhase, dSymPhase, pp;
	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS;
	double dDataI,dDataQ;

	k = nClock = nFIRIndex = M = 0;
	dAmplit = dPhase = dSymPhase = pp = 0.0;

	dwSignLen = dwDataLen/2;	

	if (m_pSgnlBuf != NULL && m_dwSgnlLen < m_nFSKDelay + dwSignLen)
	{
		delete []m_pSgnlBuf;
		m_pSgnlBuf = NULL;
	}

	if (m_pSgnlBuf == NULL)
	{
		m_dwSgnlLen = m_nFSKDelay + dwSignLen;
		m_pSgnlBuf = new double[m_nFSKDelay+dwSignLen];
	}	

	Demod_AM(pData,dwDataLen);		

	SHORT n = 0;
	// 获取前一个符号数据 //
	for(n = 0; n<m_nFSKDelay; n++)
	{
		m_pSgnlBuf[n] = m_pFSKDelay[n];
	}	
	// 去直流分量 //
	double dSignNormal = 0.0;
	for(i=0; i<dwSignLen; i++)	
	{
		dSignNormal += m_pDemodBuf[i];
	}
	dSignNormal /= dwSignLen;

	for(i=0;i<dwSignLen;i++)
	{
		m_pSgnlBuf[m_nFSKDelay+i] = m_pDemodBuf[i] - dSignNormal;
	}
	// 保留当前一个符号数据 //
	for(n = 0; n<m_nFSKDelay; n++)
	{
		m_pFSKDelay[n] = m_pSgnlBuf[dwSignLen + n];
	}
	
	dwCounter = 0;
	for(i=0;i<dwSignLen;i++)
	{
		dDataI = m_pSgnlBuf[i];
		dDataQ = m_pSgnlBuf[i + m_nFSKDelay];

		//////////////////////////////////////////
		//* AGC *//	
		dcTemp.I = dDataI*m_Agc.m;
		dcTemp.Q = dDataQ*m_Agc.m; 
		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(dDataI*dDataI + dDataQ*dDataQ);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k = m_Tlpf.nIntrFIR_sLen-1; k>0; k--)
		{
			m_Tlpf.pIntrFIR_s[k] = m_Tlpf.pIntrFIR_s[k-1];
		}
		m_Tlpf.pIntrFIR_s[0].I = dDataI*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = dDataQ*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase    = m_Tlpf.dDCO_f+m_Tlpf.dDCO_e;
		dSymPhase = 1.0-m_Tlpf.dDCO_p;

		nClock=0;
		if(dSymPhase <= dPhase)
		{
			nClock=1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;		

		if(m_Tlpf.dDCO_p>=0)
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);
		}
		else 
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p-0.5);
		}

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);	

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;

				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////符号采样////
			m_Tlpf.nIndex=(m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex==1)
			{			
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcY.I);

				dwCounter++;				

				////符号判决/////
				dcSym_d.I = dcY.I<0 ? 0:1;
				dcSym_d.Q = dcY.Q<0 ? 0:1;

				dcS.I = dcSym_d.I-m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q-m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I+m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e   = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit=sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit>1.0e-10)
				{
					pp = -20*log10(dAmplit);

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}

					m_Agc.pFIR_d[0]=pp;

					if(m_Agc.n<m_Agc.nFIRLen) m_Agc.n++;

					pp=0;

					for(k=0;k<m_Agc.nFIRLen;k++) 
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m = sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I = dcY.I;
				m_Tlpf.dcMid.Q = dcY.Q;
			}
		}		
	}	

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_FSK(double *pData,DWORD dwDataLen)
{
	DWORD i, dwSignLen, dwCounter;
	INT k,nClock,nFIRIndex,M;
	double dAmplit, dPhase, dSymPhase, pp;
	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS;
	double dDataI,dDataQ;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	dwSignLen = dwDataLen/2;	

	if (m_pSgnlBuf != NULL && m_dwSgnlLen < m_nFSKDelay + dwSignLen)
	{
		delete []m_pSgnlBuf;
		m_pSgnlBuf = NULL;
	}

	if (m_pSgnlBuf == NULL)
	{
		m_dwSgnlLen = m_nFSKDelay + dwSignLen;
		m_pSgnlBuf = new double[m_dwSgnlLen];
	}

	// 鉴频器
	Demod_FM(pData,dwDataLen);	

	SHORT n = 0;
	// 获取前一个符号数据 //
	for(n = 0; n<m_nFSKDelay; n++)
	{
		m_pSgnlBuf[n] = m_pFSKDelay[n];
	}	
	for(i=0; i < dwSignLen; i++)
	{
		m_pSgnlBuf[m_nFSKDelay+i] = m_pDemodBuf[i];
	}
	// 保留前一个符号数据 //
	for(n = 0; n<m_nFSKDelay; n++)
	{
		m_pFSKDelay[n] = m_pSgnlBuf[dwSignLen + n];
	}

	//////////////////////////////////////////////////////////////////////////

	dwCounter = 0;
	for(i=0;i<dwSignLen;i++)
	{
		dDataI = m_pSgnlBuf[i];
		dDataQ = m_pSgnlBuf[i + m_nFSKDelay];

		//////////////////////////////////////////
		//* AGC *//	
		dcTemp.I = dDataI*m_Agc.m;
		dcTemp.Q = dDataQ*m_Agc.m; 
		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(dDataI*dDataI + dDataQ*dDataQ);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k = m_Tlpf.nIntrFIR_sLen-1; k>0; k--)
		{
			m_Tlpf.pIntrFIR_s[k] = m_Tlpf.pIntrFIR_s[k-1];
		}
		m_Tlpf.pIntrFIR_s[0].I = dDataI*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = dDataQ*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase    = m_Tlpf.dDCO_f+m_Tlpf.dDCO_e;
		dSymPhase = 1.0-m_Tlpf.dDCO_p;

		nClock=0;
		if(dSymPhase <= dPhase)
		{
			nClock=1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;		

		if(m_Tlpf.dDCO_p>=0)
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);
		}
		else 
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p-0.5);
		}

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);	

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;

				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////符号采样////
			m_Tlpf.nIndex=(m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex==1)
			{			
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcY.I);

				dwCounter++;				

				////符号判决/////
				dcSym_d.I = dcY.I<0 ? 0:1;
				dcSym_d.Q = dcY.Q<0 ? 0:1;

				dcS.I = dcSym_d.I-m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q-m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I+m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e   = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit=sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit>1.0e-10)
				{
					pp = - 20*log10(dAmplit);

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}

					m_Agc.pFIR_d[0]=pp;

					if(m_Agc.n<m_Agc.nFIRLen) m_Agc.n++;

					pp=0;

					for(k=0;k<m_Agc.nFIRLen;k++) 
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m = sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I = dcY.I;
				m_Tlpf.dcMid.Q = dcY.Q;
			}
		}		
	}	

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_MSK(double *pData, DWORD dwDataLen)
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double  dAmplit, dPhase, dSymPhase, pp;

	double dDataI,dDataQ;

	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS, dcZ;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	dwCounter = 0;
	for( i = 0; i < dwDataLen/2; i++)
	{
		dDataI = pData[2*i]/dSignNormal;
		dDataQ = pData[2*i+1]/dSignNormal;

		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = dDataI*m_Agc.m;
		dcTemp.Q = dDataQ*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(dDataI*dDataI + dDataQ*dDataQ);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k]=m_Tlpf.pIntrFIR_s[k-1];
		}

		m_Tlpf.pIntrFIR_s[0].I = dDataI*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = dDataQ*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f+m_Tlpf.dDCO_e;
		dSymPhase = 1.0-m_Tlpf.dDCO_p;

		nClock=0;
		if(dSymPhase<=dPhase)
		{
			nClock=1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;		
		m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);	

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0]=dcY;

			dcY.I=dcY.Q=0.0;

			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////

			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p)+dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p)-dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			//延时1个码元并差分
			dcZ.I = m_Clpf.dcDelay[1].I;
			m_Clpf.dcDelay[1].I = m_Clpf.dcDelay[0].I;
			m_Clpf.dcDelay[0].I = dcY.I;

			dcZ.Q = m_Clpf.dcDelay[1].Q;
			m_Clpf.dcDelay[1].Q = m_Clpf.dcDelay[0].Q;
			m_Clpf.dcDelay[0].Q = dcY.Q;	

			dcTemp.I = dcY.I*dcZ.I+dcY.Q*dcZ.Q;
			dcTemp.Q = dcY.Q*dcZ.I-dcY.I*dcZ.Q;

			dcY = dcTemp;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex==1)
			{
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.Q);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*0.125*(fabs(m_Tlpf.dcMid.I+m_Tlpf.dcMid.Q)-1.0));

				dwCounter++;

				////载波误差////
				dPhase = 4*(atan2(m_Clpf.dcDelay[0].Q,m_Clpf.dcDelay[0].I)/PI + 0.0);
				while(dPhase>1.0){dPhase -= 2; }
				while(dPhase<-1.0){	dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/4;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/4;

				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit>1.0e-10)
				{
					pp = 10*log10(2.0)-20*log10(dAmplit);

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}

					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m =sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I=dcY.I;
				m_Tlpf.dcMid.Q=dcY.Q;
			}

			dPhase=m_Clpf.dNCO_p+m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_BPSK(double *pData, DWORD dwDataLen)
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double dAmplit, dPhase, dSymPhase, pp;

	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}

	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}
	
	dwCounter = 0;
	for(i=0;i<dwDataLen/2;i++)
	{
		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = pData[2*i]*m_Agc.m;
		dcTemp.Q = pData[2*i+1]*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1]);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k]=m_Tlpf.pIntrFIR_s[k-1];
		}


		m_Tlpf.pIntrFIR_s[0].I = pData[2*i]*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = pData[2*i+1]*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f+m_Tlpf.dDCO_e;
		dSymPhase = 1.0-m_Tlpf.dDCO_p;

		nClock=0;
		if(dSymPhase<=dPhase)
		{
			nClock=1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;		
		m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);	

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0]=dcY;

			dcY.I=dcY.Q=0.0;

			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////
			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p)+dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p)-dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex==1)
			{
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcY.Q);

				dwCounter++;

				////载波误差////
				dPhase = 2*atan2(dcY.Q,dcY.I)/PI;
				while(dPhase>1.0){dPhase -= 2; }
				while(dPhase<-1.0){	dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/2;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/2;

				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I + m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit > FLT_EPSILON)
				{
					pp = -20*log10(dAmplit);

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}

					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m =sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I=dcY.I;
				m_Tlpf.dcMid.Q=dcY.Q;
			}

			dPhase=m_Clpf.dNCO_p+m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_QPSK(double *pData, DWORD dwDataLen)
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double dAmplit, dPhase, dSymPhase, pp;

	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}

	dwCounter = 0;
	for(i=0;i<dwDataLen/2;i++)
	{
		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = pData[2*i]*m_Agc.m;
		dcTemp.Q = pData[2*i+1]*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1]);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k] = m_Tlpf.pIntrFIR_s[k-1];
		}

		m_Tlpf.pIntrFIR_s[0].I = pData[2*i]*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = pData[2*i+1]*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f+m_Tlpf.dDCO_e;
		dSymPhase = 1.0-m_Tlpf.dDCO_p;

		nClock = 0;
		if(dSymPhase<=dPhase)
		{
			nClock = 1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;		
		if(m_Tlpf.dDCO_p>=0)
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);
		}
		else 
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p-0.5);
		}

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0] = dcY;

			dcY.I = dcY.Q = 0.0;

			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////
			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p)+dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p)-dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex==1)
			{
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcY.Q);

				dwCounter++;

				////载波误差////
				dPhase = 4*(atan2(dcY.Q,dcY.I)/PI + 0.25);
				while(dPhase>1.0){dPhase -= 2; }
				while(dPhase<-1.0){	dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/4;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/4;

				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I + m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit > FLT_EPSILON)
				{
					pp = 10*log10(2.0) - 20*log10(dAmplit);

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}

					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m = sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I=dcY.I;
				m_Tlpf.dcMid.Q=dcY.Q;
			}

			dPhase=m_Clpf.dNCO_p+m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_8PSK(double *pData, DWORD dwDataLen)
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double dAmplit, dPhase, dSymPhase, pp;

	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}

	dwCounter = 0;
	for(i=0;i<dwDataLen/2;i++)
	{
		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = pData[2*i]*m_Agc.m;
		dcTemp.Q = pData[2*i+1]*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I + dcTemp.Q*dcTemp.Q);
		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(pData[2*i]*pData[2*i] + pData[2*i+1]*pData[2*i+1]);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k] = m_Tlpf.pIntrFIR_s[k-1];
		}
		m_Tlpf.pIntrFIR_s[0].I = pData[2*i]*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = pData[2*i+1]*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f + m_Tlpf.dDCO_e;
		dSymPhase = 1.0 - m_Tlpf.dDCO_p;

		nClock = 0;
		if(dSymPhase <= dPhase)
		{
			nClock = 1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}		

		m_Tlpf.dDCO_p += dPhase;	
		if(m_Tlpf.dDCO_p >= 0)
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);
		}
		else 
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p - 0.5);
		}		

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);
			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0] = dcY;

			dcY.I = dcY.Q = 0.0;
			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////
			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p) + dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p) - dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex == 1)
			{
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcY.Q);

				dwCounter++;

				////载波误差////
				dPhase = 8*(atan2(dcY.Q,dcY.I)/PI + 0.125);
				while(dPhase>1.0){dPhase -= 2; }
				while(dPhase<-1.0){	dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/8;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/8;

				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I + m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit > FLT_EPSILON)
				{
					pp = 10*log10(2.0) - 20*log10(dAmplit);

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}
					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m = sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I=dcY.I;
				m_Tlpf.dcMid.Q=dcY.Q;
			}

			dPhase = m_Clpf.dNCO_p + m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}
	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_16PSK(double *pData, DWORD dwDataLen)
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double dAmplit, dPhase, dSymPhase, pp;

	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}

	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}

	dwCounter = 0;
	for(i=0;i<dwDataLen/2;i++)
	{
		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = pData[2*i]*m_Agc.m;
		dcTemp.Q = pData[2*i+1]*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1]);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k] = m_Tlpf.pIntrFIR_s[k-1];
		}

		m_Tlpf.pIntrFIR_s[0].I = pData[2*i]*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = pData[2*i+1]*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f + m_Tlpf.dDCO_e;
		dSymPhase = 1.0 - m_Tlpf.dDCO_p;

		nClock = 0;
		if(dSymPhase <= dPhase)
		{
			nClock = 1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}
		else
		{
			nClock = 0;
		}

		m_Tlpf.dDCO_p += dPhase;	
		if(m_Tlpf.dDCO_p >= 0)
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);
		}
		else 
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p-0.5);
		}		

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0] = dcY;

			dcY.I = dcY.Q = 0.0;

			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////
			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p) + dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p) - dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex == 1)
			{
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcY.Q);

				dwCounter++;

				////载波误差////
				dPhase = 16*(atan2(dcY.Q,dcY.I)/PI + 0.0625);
				while(dPhase>1.0){dPhase -= 2; }
				while(dPhase<-1.0){	dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/16;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/16;

				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I + m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit > FLT_EPSILON)
				{
					pp = 10*log10(2.0) - 20*log10(dAmplit);

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}
					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m = sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I=dcY.I;
				m_Tlpf.dcMid.Q=dcY.Q;
			}

			dPhase = m_Clpf.dNCO_p + m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_OQPSK(double *pData,DWORD dwDataLen)
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double dAmplit, dPhase, dSymPhase, pp;

	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS, dcZ;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}

	dwCounter = 0;
	for(i=0;i<dwDataLen/2;i++)
	{
		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = pData[2*i]*m_Agc.m;
		dcTemp.Q = pData[2*i+1]*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1]);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k]=m_Tlpf.pIntrFIR_s[k-1];
		}

		m_Tlpf.pIntrFIR_s[0].I = pData[2*i]*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = pData[2*i+1]*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f + m_Tlpf.dDCO_e;
		dSymPhase = 1.0 - m_Tlpf.dDCO_p;

		nClock=0;
		if(dSymPhase<=dPhase)
		{
			nClock=1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;		
		m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);	

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0] = dcY;

			dcY.I = dcY.Q = 0.0;

			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////
			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p)+dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p)-dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			// Q路延时1个码元 //
			dcZ.I = dcY.I;
			dcZ.Q = m_Clpf.dDelay;			
			m_Clpf.dDelay = dcY.Q;

			dcY = dcZ;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex==1)
			{
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcY.Q);

				dwCounter++;

				////载波误差////
				dPhase = 4*(atan2(dcY.Q,dcY.I)/PI+0.25);
				while(dPhase>1.0){dPhase -= 2; }
				while(dPhase<-1.0){	dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/4;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/4;

				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I + m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit > FLT_EPSILON)
				{
					pp = 10*log10(2.0) - 20*log10(dAmplit);

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}

					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m =sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I=dcY.I;
				m_Tlpf.dcMid.Q=dcY.Q;
			}

			dPhase = m_Clpf.dNCO_p + m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_SDPSK( double *pData,DWORD dwDataLen )
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double dAmplit, dPhase, dSymPhase, pp;

	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS, dcZ;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}

	dwCounter = 0;
	for(i=0;i<dwDataLen/2;i++)
	{
		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = pData[2*i]*m_Agc.m;
		dcTemp.Q = pData[2*i+1]*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1]);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k]=m_Tlpf.pIntrFIR_s[k-1];
		}

		m_Tlpf.pIntrFIR_s[0].I = pData[2*i]*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = pData[2*i+1]*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f+m_Tlpf.dDCO_e;
		dSymPhase = 1.0-m_Tlpf.dDCO_p;

		nClock=0;
		if(dSymPhase<=dPhase)
		{
			nClock=1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;		
		m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);	

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0]=dcY;

			dcY.I=dcY.Q=0.0;

			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////
			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p)+dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p)-dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex==1)
			{
				//延时1个码元并差分
				dcZ.I = m_Clpf.dcDelay[1].I;
				m_Clpf.dcDelay[1].I = m_Clpf.dcDelay[0].I;
				m_Clpf.dcDelay[0].I = dcY.I;

				dcZ.Q = m_Clpf.dcDelay[1].Q;
				m_Clpf.dcDelay[1].Q = m_Clpf.dcDelay[0].Q;
				m_Clpf.dcDelay[0].Q = dcY.Q;	

				dcTemp.I = dcY.I*dcZ.I+dcY.Q*dcZ.Q;
				dcTemp.Q = dcY.Q*dcZ.I-dcY.I*dcZ.Q;				

				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcTemp.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcTemp.Q);

				dwCounter++;

				////载波误差////
				dPhase = 2*atan2(dcY.Q,dcY.I)/PI;
				while(dPhase>1.0){dPhase -= 2; }
				while(dPhase<-1.0){	dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/2;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/2;

				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I + m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit>1.0e-10)
				{
					pp = -20*log10(dAmplit);

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}

					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m =sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I=dcY.I;
				m_Tlpf.dcMid.Q=dcY.Q;
			}

			dPhase=m_Clpf.dNCO_p+m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}


DWORD CSignalDemodProbe::Demod_PI4DQPSK(double *pData, DWORD dwDataLen)
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double dAmplit, dPhase, dSymPhase, pp;

	COMPLEX_SIGNAL dcTemp, dcY,  dcZ, dcSym_d, dcS;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}

	dwCounter = 0;
	for(i=0;i<dwDataLen/2;i++)
	{
		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = pData[2*i]*m_Agc.m;
		dcTemp.Q = pData[2*i+1]*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1]);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k]=m_Tlpf.pIntrFIR_s[k-1];
		}

		m_Tlpf.pIntrFIR_s[0].I = pData[2*i]*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = pData[2*i+1]*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f+m_Tlpf.dDCO_e;
		dSymPhase = 1.0-m_Tlpf.dDCO_p;

		nClock = 0;
		if(dSymPhase<=dPhase)
		{
			nClock = 1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;	
		if(m_Tlpf.dDCO_p >= 0)
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);
		}
		else 
		{
			m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p-0.5);
		}

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0] = dcY;

			dcY.I=dcY.Q=0.0;

			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////
			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p)+dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p)-dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex==1)
			{
				//延时1个码元并差分
				dcZ.I = m_Clpf.dcDelay[1].I;
				m_Clpf.dcDelay[1].I = m_Clpf.dcDelay[0].I;
				m_Clpf.dcDelay[0].I = dcY.I;

				dcZ.Q = m_Clpf.dcDelay[1].Q;
				m_Clpf.dcDelay[1].Q = m_Clpf.dcDelay[0].Q;
				m_Clpf.dcDelay[0].Q = dcY.Q;	

				dcTemp.I = dcY.I*dcZ.I+dcY.Q*dcZ.Q;
				dcTemp.Q = dcY.Q*dcZ.I-dcY.I*dcZ.Q;

				dcZ.I = dcTemp.I*cos(PI/4) + dcTemp.Q*sin(PI/4);
				dcZ.Q = dcTemp.Q*cos(PI/4) - dcTemp.I*sin(PI/4);

				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcZ.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcZ.Q);

				dwCounter++;

				////载波误差////
				dPhase = 4*(atan2(dcY.Q,dcY.I)/PI + 0.25);
				while(dPhase>1.0){dPhase -= 2; }
				while(dPhase<-1.0){	dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/4;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/4;

				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I + m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit > FLT_EPSILON)
				{
					pp = 10*log10(2.0)-20*log10(dAmplit);

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}
					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m =sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I=dcY.I;
				m_Tlpf.dcMid.Q=dcY.Q;
			}

			dPhase=m_Clpf.dNCO_p+m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}


DWORD CSignalDemodProbe::Demod_16QAM(double *pData, DWORD dwDataLen)
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double dAmplit, dPhase, dSymPhase, pp;

	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}
	//////////////////////////////////////////////////////////////////////////

	dwCounter = 0;
	for(i=0;i<dwDataLen/2;i++)
	{
		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = pData[2*i]*m_Agc.m;
		dcTemp.Q = pData[2*i+1]*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1]);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k]=m_Tlpf.pIntrFIR_s[k-1];
		}

		m_Tlpf.pIntrFIR_s[0].I = pData[2*i]*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = pData[2*i+1]*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f+m_Tlpf.dDCO_e;
		dSymPhase = 1.0-m_Tlpf.dDCO_p;

		nClock=0;
		if(dSymPhase<=dPhase)
		{
			nClock=1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;		
		m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);	

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0]=dcY;

			dcY.I=dcY.Q=0.0;

			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////
			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p)+dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p)-dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex == 1)
			{
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcY.Q);

				dwCounter++;

				////载波误差////
				dPhase = atan2(dcY.Q,dcY.I)/PI;

				if( (fabs(dcY.I)>0.5 && fabs(dcY.Q)>0.5) 
					|| (fabs(dcY.I)<=0.5 && fabs(dcY.Q)<=0.5) )
				{
					dPhase = 4*(dPhase + 0.25);
				}
				else if( (fabs(dcY.I)>fabs(dcY.Q) && dcY.I*dcY.Q>=0) 
					|| (fabs(dcY.I)<fabs(dcY.Q) && dcY.I*dcY.Q<=0) )
				{
					dPhase = 4*(dPhase - 0.10241638234956672582459892377526);
				}
				else if( (fabs(dcY.I)>fabs(dcY.Q) && dcY.I*dcY.Q<=0) 
					|| (fabs(dcY.I)<fabs(dcY.Q) && dcY.I*dcY.Q>=0) )
				{
					dPhase = 4*(dPhase + 0.10241638234956672582459892377526);
				}
				else
				{
					dPhase = 0.0;
				}
				while(dPhase>1.0){dPhase -= 2;}
				while(dPhase<-1.0){dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/4;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/4;

				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I + m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc+dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit>1.0e-10)
				{
					pp = 20*log10(dAmplit);

					if( fabs(dcY.I)<=0.5 && fabs(dcY.Q)<=0.5 )
					{
						pp=10.0*log10(0.25*0.25*2.0)- pp;
					}
					else if( fabs(dcY.I)>0.5 && fabs(dcY.Q)>0.5 )
					{
						pp=10.0*log10(0.75*0.75*2.0)- pp;
					}
					else
					{
						pp=10.0*log10(0.75*0.75 + 0.25*0.25)- pp;
					}

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}

					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m =sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I=dcY.I;
				m_Tlpf.dcMid.Q=dcY.Q;
			}

			dPhase = m_Clpf.dNCO_p + m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_32QAM( double *pData,DWORD dwDataLen )
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double dAmplit, dPhase, dSymPhase, pp, uu;

	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS;	

	SHORT cntCP = 7;
	double CaliPhase[]={0.25,0.102416382349567,0.0628329581890012,
		0.0451672353008665,0.172020869622631,0.128881058409157,
		0.197431543288747};
	SHORT cntCA = 10;
	double CaliAmpl[]={0.25*0.25*2,0.75*0.75*2,1.25*1.25*2,1.75*1.75*2,
		0.25*0.25+0.75*0.75,0.25*0.25+1.25*1.25,0.25*0.25+1.75*1.75,
		0.75*0.75+1.25*1.25,0.75*0.75+1.75*1.75,1.25*1.25+1.75*1.75 };

	SHORT nMinIdx = 0;
	double dMinVal = 0;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}
	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}
	dwCounter = 0;
	for(i=0;i<dwDataLen/2;i++)
	{
		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = pData[2*i]*m_Agc.m;
		dcTemp.Q = pData[2*i+1]*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1]);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k]=m_Tlpf.pIntrFIR_s[k-1];
		}

		m_Tlpf.pIntrFIR_s[0].I = pData[2*i]*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = pData[2*i+1]*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f+m_Tlpf.dDCO_e;
		dSymPhase = 1.0-m_Tlpf.dDCO_p;

		nClock=0;
		if(dSymPhase<=dPhase)
		{
			nClock=1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;		
		m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);	

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0]=dcY;

			dcY.I = dcY.Q = 0.0;

			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////
			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p)+dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p)-dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex == 1)
			{
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcY.Q);

				dwCounter++;

				////提取载波误差////
				dPhase = atan2(dcY.Q,dcY.I)/PI;

				uu = atan2(fabs(dcY.Q),fabs(dcY.I))/PI;

				dMinVal = fabs(uu - CaliPhase[0]);
				nMinIdx = 0;
				for (SHORT kr = 1; kr < cntCP; kr++)
				{
					if (dMinVal > fabs(uu - CaliPhase[kr]))
					{
						dMinVal = fabs(uu - CaliPhase[kr]);

						nMinIdx = kr;
					}
				}

				if( nMinIdx == 0  )
				{
					dPhase = 4*(dPhase + 0.25);
				}
				else if( (fabs(dcY.I)>fabs(dcY.Q) && dcY.I*dcY.Q>=0) 
					|| (fabs(dcY.I)<fabs(dcY.Q) && dcY.I*dcY.Q<=0) )
				{
					dPhase = 4*(dPhase - CaliPhase[nMinIdx]);
				}
				else if( (fabs(dcY.I)>fabs(dcY.Q) && dcY.I*dcY.Q<=0) 
					|| (fabs(dcY.I)<fabs(dcY.Q) && dcY.I*dcY.Q>=0) )
				{
					dPhase = 4*(dPhase + CaliPhase[nMinIdx]);
				}
				else
				{
					dPhase=0.0;
				}

				while(dPhase>1.0){dPhase -= 2;}
				while(dPhase<-1.0){dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/4;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/4;

				/////////////////////////////////////////////////////
				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I + m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc + dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit>1.0e-10)
				{
					// 提取AGC误差 //
					pp = 20*log10(dAmplit);

					dMinVal = fabs(uu - CaliAmpl[0]);
					nMinIdx = 0;
					for (SHORT kr = 1; kr < cntCA; kr++)
					{
						if (dMinVal > fabs(pp - CaliAmpl[kr]))
						{
							dMinVal = fabs(pp - CaliAmpl[kr]);

							nMinIdx = kr;
						}
					}

					pp = 10.0*log10(CaliAmpl[nMinIdx])- pp;

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}
					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m =sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I = dcY.I;
				m_Tlpf.dcMid.Q = dcY.Q;
			}

			dPhase=m_Clpf.dNCO_p+m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

DWORD CSignalDemodProbe::Demod_64QAM( double *pData,DWORD dwDataLen )
{
	DWORD i,dwCounter;
	INT k,nClock,nFIRIndex,M;	

	double dAmplit, dPhase, dSymPhase, pp, uu;

	COMPLEX_SIGNAL dcTemp, dcY, dcSym_d, dcS;	

	SHORT cntCP = 7;
	double CaliPhase[]={0.25,0.102416382349567,0.0628329581890012,
		0.0451672353008665,0.172020869622631,0.128881058409157,
		0.197431543288747};
	SHORT cntCA = 10;
	double CaliAmpl[]={0.25*0.25*2,0.75*0.75*2,1.25*1.25*2,1.75*1.75*2,
		0.25*0.25+0.75*0.75,0.25*0.25+1.25*1.25,0.25*0.25+1.75*1.75,
		0.75*0.75+1.25*1.25,0.75*0.75+1.75*1.75,1.25*1.25+1.75*1.75 };

	SHORT nMinIdx = 0;
	double dMinVal = 0;

	k = nClock = nFIRIndex = M = 0;

	dAmplit = dPhase = dSymPhase = pp = 0.0;

	////计算信号规一化参数////
	double dSignNormal=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dSignNormal += pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1];
	}

	dSignNormal = sqrt(dSignNormal/dwDataLen) + FLT_EPSILON;	

	for(i=0;i<dwDataLen;i++)
	{
		pData[i] = pData[i]/dSignNormal;
	}
	dwCounter = 0;
	for(i=0;i<dwDataLen/2;i++)
	{
		//////////////////////////////////////////
		//* AGC *//
		dcTemp.I = pData[2*i]*m_Agc.m;
		dcTemp.Q = pData[2*i+1]*m_Agc.m; 

		dAmplit = sqrt(dcTemp.I*dcTemp.I+dcTemp.Q*dcTemp.Q);

		if(dAmplit>4)
		{
			m_Agc.m = 4/sqrt(pData[2*i]*pData[2*i]+pData[2*i+1]*pData[2*i+1]);
			m_Agc.s = 20*log10(m_Agc.m);
		}

		for(k=m_Tlpf.nIntrFIR_sLen-1;k>0;k--)
		{
			m_Tlpf.pIntrFIR_s[k]=m_Tlpf.pIntrFIR_s[k-1];
		}

		m_Tlpf.pIntrFIR_s[0].I = pData[2*i]*m_Agc.m;
		m_Tlpf.pIntrFIR_s[0].Q = pData[2*i+1]*m_Agc.m;

		///////////////////////////////////////////////
		//* DCO *//
		dPhase = m_Tlpf.dDCO_f+m_Tlpf.dDCO_e;
		dSymPhase = 1.0-m_Tlpf.dDCO_p;

		nClock=0;
		if(dSymPhase<=dPhase)
		{
			nClock=1;
			dSymPhase /= dPhase;			
			dSymPhase -= int(dSymPhase);			
		}

		m_Tlpf.dDCO_p += dPhase;		
		m_Tlpf.dDCO_p -= int(m_Tlpf.dDCO_p);	

		////定时////
		if(nClock)
		{
			dcY.I = dcY.Q = 0.0;

			////内插抽取滤波/////
			nFIRIndex = int(dSymPhase*64);

			for(k=0;k<m_Tlpf.nIntrFIR_sLen;k++)
			{
				M = m_Tlpf.nIntrFIR_sLen*nFIRIndex+k;
				dcY.I += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].I;
				dcY.Q += m_Tlpf.pIntrFIR_b[M]*m_Tlpf.pIntrFIR_s[k].Q;
			}

			////匹配滤波////
			for(k=m_Tlpf.nMatchFIR_Len-1;k>0;k--)	
			{
				m_Tlpf.pMatcFIR_s[k]=m_Tlpf.pMatcFIR_s[k-1];
			}
			m_Tlpf.pMatcFIR_s[0]=dcY;

			dcY.I = dcY.Q = 0.0;

			for(k=0;k<m_Tlpf.nMatchFIR_Len;k++)
			{
				dcY.I += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].I;
				dcY.Q += m_Tlpf.pMatcFIR_b[k]*m_Tlpf.pMatcFIR_s[k].Q;
			}

			////载波校正////
			dcTemp.I = dcY.I*cos(PI*m_Clpf.dNCO_p)+dcY.Q*sin(PI*m_Clpf.dNCO_p);
			dcTemp.Q = dcY.Q*cos(PI*m_Clpf.dNCO_p)-dcY.I*sin(PI*m_Clpf.dNCO_p);

			dcY = dcTemp;

			////符号采样////
			m_Tlpf.nIndex = (m_Tlpf.nIndex+1)%2;

			if(m_Tlpf.nIndex == 1)
			{
				////符号/////
				m_pDemodBuf[2*dwCounter] = LONG(SHRT_MAX*dcY.I);
				m_pDemodBuf[2*dwCounter+1] = LONG(SHRT_MAX*dcY.Q);

				dwCounter++;

				////提取载波误差////
				dPhase = atan2(dcY.Q,dcY.I)/PI;

				uu = atan2(fabs(dcY.Q),fabs(dcY.I))/PI;

				dMinVal = fabs(uu - CaliPhase[0]);
				nMinIdx = 0;
				for (SHORT kr = 1; kr < cntCP; kr++)
				{
					if (dMinVal > fabs(uu - CaliPhase[kr]))
					{
						dMinVal = fabs(uu - CaliPhase[kr]);

						nMinIdx = kr;
					}
				}

				if( nMinIdx == 0  )
				{
					dPhase = 4*(dPhase + 0.25);
				}
				else if( (fabs(dcY.I)>fabs(dcY.Q) && dcY.I*dcY.Q>=0) 
					|| (fabs(dcY.I)<fabs(dcY.Q) && dcY.I*dcY.Q<=0) )
				{
					dPhase = 4*(dPhase - CaliPhase[nMinIdx]);
				}
				else if( (fabs(dcY.I)>fabs(dcY.Q) && dcY.I*dcY.Q<=0) 
					|| (fabs(dcY.I)<fabs(dcY.Q) && dcY.I*dcY.Q>=0) )
				{
					dPhase = 4*(dPhase + CaliPhase[nMinIdx]);
				}
				else
				{
					dPhase=0.0;
				}

				while(dPhase>1.0){dPhase -= 2;}
				while(dPhase<-1.0){dPhase += 2;}

				////环路滤波////
				m_Clpf.dLpf_cc += dPhase*m_Clpf.dLpf_c2/4;
				m_Clpf.dNCO_e = m_Clpf.dLpf_cc + dPhase*m_Clpf.dLpf_c1/4;

				/////////////////////////////////////////////////////
				////符号判决/////
				dcSym_d.I=1;
				dcSym_d.Q=1;
				if(dcY.I<0)dcSym_d.I=0;
				if(dcY.Q<0)dcSym_d.Q=0;

				dcS.I = dcSym_d.I - m_Tlpf.dcDelay.I;
				dcS.Q = dcSym_d.Q - m_Tlpf.dcDelay.Q;

				m_Tlpf.dcDelay = dcSym_d;

				////定时误差////
				if(dcS.I<0)			dcS.I=-1.0;
				else if(dcS.I>0)	dcS.I=1.0;
				else				dcS.I=0.0;

				if(dcS.Q<0)			dcS.Q=-1.0;
				else if(dcS.Q>0)	dcS.Q=1.0;
				else				dcS.Q=0.0;

				dPhase = m_Tlpf.dcMid.I*dcS.I + m_Tlpf.dcMid.Q*dcS.Q;

				////环路滤波////
				m_Tlpf.dLpf_cc += dPhase*m_Tlpf.dLpf_c2;
				m_Tlpf.dDCO_e = m_Tlpf.dLpf_cc + dPhase*m_Tlpf.dLpf_c1;

				//* AGC *//
				dAmplit = sqrt(dcY.I*dcY.I+dcY.Q*dcY.Q);

				if(dAmplit>1.0e-10)
				{
					// 提取AGC误差 //
					pp = 20*log10(dAmplit);

					dMinVal = fabs(uu - CaliAmpl[0]);
					nMinIdx = 0;
					for (SHORT kr = 1; kr < cntCA; kr++)
					{
						if (dMinVal > fabs(pp - CaliAmpl[kr]))
						{
							dMinVal = fabs(pp - CaliAmpl[kr]);

							nMinIdx = kr;
						}
					}

					pp = 10.0*log10(CaliAmpl[nMinIdx])- pp;

					////AGC滤波////
					for(k=m_Agc.nFIRLen-1;k>0;k--)
					{
						m_Agc.pFIR_d[k]=m_Agc.pFIR_d[k-1];
					}
					m_Agc.pFIR_d[0] = pp;

					if(m_Agc.n<m_Agc.nFIRLen) 
					{
						m_Agc.n++;
					}

					pp=0;
					for(k=0;k<m_Agc.nFIRLen;k++)
					{
						pp += m_Agc.pFIR_d[k];
					}
					pp /= m_Agc.n;

					////AGC误差////
					pp *= m_Agc.a;
					m_Agc.s += pp;
					m_Agc.m =sqrt(pow(10.0,0.1*m_Agc.s));					
				}	          			
			}
			else
			{
				m_Tlpf.dcMid.I = dcY.I;
				m_Tlpf.dcMid.Q = dcY.Q;
			}

			dPhase=m_Clpf.dNCO_p+m_Clpf.dNCO_e;

			if(dPhase>=1)dPhase -= 2.0;
			if(dPhase<-1)dPhase += 2.0; 

			m_Clpf.dNCO_p = dPhase;
		}		
	}

	m_dwSylNum = 2*dwCounter;

	return(ERROR_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////

// 输入解调后IQ符号
DWORD CSignalDemodProbe::InputSignal(LONG *pData, DWORD dwDataLen)
{
	DWORD dwErrorID = ERROR_SUCCESS;

	switch (m_nModuType)
	{	
	case SMT_MSK:
	case SMT_GMSK:
	case SMT_2ASK:
	case SMT_2FSK:
	case SMT_BPSK:
	case SMT_SDPSK:
		{
			dwErrorID = Decode_2Lvl(pData,dwDataLen);
		}
		break;
	case SMT_4ASK:
	case SMT_4FSK:
		{
			dwErrorID = Decode_4Lvl(pData,dwDataLen);
		}
		break;	
	case SMT_QPSK:	
	case SMT_OQPSK:	
	case SMT_PI4DQPSK:
		{
			dwErrorID = Decode_4Phase(pData,dwDataLen);
		}
		break;
	case SMT_8QAM:
	case SMT_8PSK:
		{
			dwErrorID = Decode_8PSK(pData,dwDataLen);
		}
		break;
	case SMT_16PSK:
		{
			dwErrorID = Decode_16PSK(pData,dwDataLen);
		}
		break;
	case SMT_16APSK:
		{
			dwErrorID = Decode_16APSK(pData,dwDataLen);
		}
		break;
	case SMT_16QAM:
		{
			dwErrorID = Decode_16QAM(pData,dwDataLen);
		}
		break;
	case SMT_32QAM:
		{
			dwErrorID = Decode_32QAM(pData,dwDataLen);
		}
		break;
	case SMT_64QAM:
		{
			dwErrorID = Decode_64QAM(pData,dwDataLen);
		}
		break;
	default:
		{
			dwErrorID = ERROR_UNDEFINED_DECODER_TYPE;
		}
		break;
	}

	return(dwErrorID);
}

// MSK、2FSK、BPSK译码
DWORD CSignalDemodProbe::Decode_2Lvl(LONG *pData, DWORD dwDataLen)
{
	DWORD dwErrorID = ERROR_SUCCESS;

	if (m_pBitBuf != NULL && m_dwBufLen < dwDataLen/2)
	{
		delete []m_pBitBuf;

		m_pBitBuf = NULL;
	}	

	if (m_pBitBuf == NULL)
	{
		m_dwBufLen = dwDataLen/2;

		m_pBitBuf = new CHAR[m_dwBufLen];
	}

	m_dwBitNum = dwDataLen/2;
	//ZeroMemory(m_pBitBuf,m_dwBufLen*sizeof(CHAR));//

	for (DWORD i = 0; i < dwDataLen/2; i++)
	{
		if (*(pData + 2*i) > 0)
		{
			m_pBitBuf[i] = '1';
		}
		else
			m_pBitBuf[i] = '0';
	}

	// 差分译码 //
	if (m_bIsDecode)
	{
		CHAR byMod2Sum;

		////求本次输入第一个点的差分码////
		byMod2Sum = (m_pBitBuf[0] + m_PreCode[0])%2;

		if( byMod2Sum == 0 )
		{
			m_pBitBuf[0] = 1;//
		}
		else
		{
			m_pBitBuf[0] = 0;//
		}

		////求差分码////
		for(DWORD i=1;i<dwDataLen/2;i++)
		{
			byMod2Sum = (m_pBitBuf[i] + m_pBitBuf[i-1])%2;

			if( byMod2Sum == 0 )
			{
				m_pBitBuf[i]  = 1;//
			}
			else
			{
				m_pBitBuf[i]  = 0;//
			}
		}

		////保存本次输入最后一个点的检测码////
		m_PreCode[0] = m_pBitBuf[m_dwBitNum-1];	
	}	

	return(dwErrorID);
}
DWORD CSignalDemodProbe::Decode_4Lvl(LONG *pData, DWORD dwDataLen)
{
	DWORD i = 0;

	DWORD dwErrorID = ERROR_SUCCESS;

	if (m_pSgnlBuf != NULL && m_dwSgnlLen < dwDataLen/2)
	{
		delete []m_pSgnlBuf;

		m_pSgnlBuf = NULL;
	}	

	if (m_pSgnlBuf == NULL)
	{
		m_dwSgnlLen = dwDataLen/2;

		m_pSgnlBuf = new double[m_dwSgnlLen];
	}

	if (m_pBitBuf != NULL && m_dwBufLen < dwDataLen)
	{
		delete []m_pBitBuf;

		m_pBitBuf = NULL;
	}	

	if (m_pBitBuf == NULL)
	{
		m_dwBufLen = dwDataLen;

		m_pBitBuf = new CHAR[m_dwBufLen];
	}

	m_dwBitNum = dwDataLen;
	ZeroMemory(m_pBitBuf,m_dwBufLen*sizeof(CHAR));	

	////求数据归一化因子///	
	double dMeanValue=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dMeanValue += pData[2*i]*pData[2*i];
	}
	dMeanValue /= dwDataLen/2;

	if (dMeanValue < 1e-5)
	{
		return(dwErrorID);
	}

	////数据归一化///
	for(i=0;i<dwDataLen/2;i++)
	{
		m_pSgnlBuf[i] = pData[2*i]*sqrt(5.0)/sqrt(dMeanValue);
	}

	// 格雷码逻辑映射 //
	for (i = 0; i < dwDataLen/2; i++)
	{	
		if((m_pSgnlBuf[i] > 2))	// +3
		{
			m_pBitBuf[2*i]  = 0;//1;
			m_pBitBuf[2*i+1]= 0;
		}		
		else if(m_pSgnlBuf[i]>0 && m_pSgnlBuf[i]<2)			// +1
		{
			m_pBitBuf[2*i]  = 0;//1;
			m_pBitBuf[2*i+1]= 1;
		}
		else if(m_pSgnlBuf[i]<0 && m_pSgnlBuf[i]>-2)		// -1
		{
			m_pBitBuf[2*i]  = 1;//0;
			m_pBitBuf[2*i+1]= 1;
		}
		else //if((m_pSgnlBuf[i] < -2))	// -3
		{
			m_pBitBuf[2*i]  = 1;//0;
			m_pBitBuf[2*i+1]= 0;
		}
	}	

	return(dwErrorID);
}
// QPSK、OQPSK、PI/4DQPSK 译码
DWORD CSignalDemodProbe::Decode_4Phase(LONG *pData, DWORD dwDataLen)
{
	DWORD dwErrorID = ERROR_SUCCESS;

	if (m_pBitBuf != NULL && m_dwBufLen < dwDataLen)
	{
		delete []m_pBitBuf;

		m_pBitBuf = NULL;
	}	

	if (m_pBitBuf == NULL)
	{
		m_dwBufLen = dwDataLen;

		m_pBitBuf = new CHAR[m_dwBufLen];
	}

	m_dwBitNum = dwDataLen;
	ZeroMemory(m_pBitBuf,m_dwBufLen*sizeof(CHAR));

	// 格雷码逻辑映射 //
	for (DWORD i = 0; i < dwDataLen/2; i++)
	{		
		if(pData[2*i]<0 && pData[2*i+1]>0)		//第二象限
		{
			m_pBitBuf[2*i]  = '0';
			m_pBitBuf[2*i+1]= '1';
		}
		else if(pData[2*i]<0 && pData[2*i+1]<0)	//第三象限
		{
			m_pBitBuf[2*i]  = '1';
			m_pBitBuf[2*i+1]= '1';
		}
		else if(pData[2*i]>0 && pData[2*i+1]<0)	//第四象限
		{
			m_pBitBuf[2*i]  = '1';
			m_pBitBuf[2*i+1]= '0';
		}
		else //(pData[2*i]>0 && pData[2*i+1]>0)	//第一象限
		{
			m_pBitBuf[2*i]  = '0';
			m_pBitBuf[2*i+1]= '0';
		}		
	}	

	// 差分译码 //
	if (m_bIsDecode)
	{
		CHAR byMod2Sum,byCode0,byCode1;

		////求本次输入第一个点的差分码////
		byMod2Sum = (m_pBitBuf[0]+m_pBitBuf[1]+m_PreCode[0]+m_PreCode[1])%2;

		if( byMod2Sum == 0 )
		{
			byCode0 = (m_pBitBuf[0] + m_PreCode[0])%2;
			byCode1 = (m_pBitBuf[1] + m_PreCode[1])%2;
		}
		else
		{
			byCode0 =(m_pBitBuf[0] + m_PreCode[1])%2;
			byCode1 =(m_pBitBuf[1] + m_PreCode[0])%2;
		}

		m_pBitBuf[0] = byCode0;
		m_pBitBuf[1] = byCode1;

		////求差分码////
		for(DWORD i=1;i<dwDataLen/2;i++)
		{
			byMod2Sum = (m_pBitBuf[2*i] + m_pBitBuf[2*i+1] + m_pBitBuf[2*(i-1)]
			+ m_pBitBuf[2*(i-1)+1])%2;

			if( byMod2Sum == 0 )
			{
				byCode0 = (m_pBitBuf[2*i] + m_pBitBuf[2*(i-1)])%2;
				byCode1 = (m_pBitBuf[2*i+1] + m_pBitBuf[2*(i-1)+1])%2;
			}
			else
			{
				byCode0 = (m_pBitBuf[2*i] + m_pBitBuf[2*(i-1)+1])%2;
				byCode1 = (m_pBitBuf[2*i+1] + m_pBitBuf[2*(i-1)])%2;
			}

			m_pBitBuf[2*i]  = byCode0;
			m_pBitBuf[2*i+1]= byCode1;
		}

		////保存本次输入最后一个点的检测码////
		m_PreCode[0] = m_pBitBuf[m_dwBitNum-2];
		m_PreCode[1] = m_pBitBuf[m_dwBitNum-1];		
	}

	return(dwErrorID);
}

// 8PSK
DWORD CSignalDemodProbe::Decode_8PSK(LONG *pData, DWORD dwDataLen)
{
	DWORD dwErrorID = ERROR_SUCCESS;

	if (m_pBitBuf != NULL && m_dwBufLen < 3*dwDataLen/2)
	{
		delete []m_pBitBuf;

		m_pBitBuf = NULL;
	}	

	if (m_pBitBuf == NULL)
	{
		m_dwBufLen = 3*dwDataLen/2;

		m_pBitBuf = new CHAR[m_dwBufLen];
	}

	m_dwBitNum = 3*dwDataLen/2;
	ZeroMemory(m_pBitBuf,m_dwBufLen*sizeof(CHAR));

	// 格雷码逻辑映射
	for (DWORD i = 0; i < dwDataLen/2; i++)
	{		
		if( pData[2*i]>0 && pData[2*i+1]>0 && abs(pData[2*i])<abs(pData[2*i+1]) )	//1
		{
			m_pBitBuf[3*i]  = 0;
			m_pBitBuf[3*i+1]= 0;
			m_pBitBuf[3*i+2]= 1;
		}
		else if( pData[2*i]<0 && pData[2*i+1]>0 && abs(pData[2*i])<abs(pData[2*i+1]) )	//3
		{
			m_pBitBuf[3*i]  = 0;
			m_pBitBuf[3*i+1]= 1;
			m_pBitBuf[3*i+2]= 1;
		}
		else if( pData[2*i]<0 && pData[2*i+1]>0 && abs(pData[2*i])>abs(pData[2*i+1]) )	//2
		{
			m_pBitBuf[3*i]  = 0;
			m_pBitBuf[3*i+1]= 1;
			m_pBitBuf[3*i+2]= 0;
		}
		else if( pData[2*i]<0 && pData[2*i+1]<0 && abs(pData[2*i])>abs(pData[2*i+1]))		//6
		{
			m_pBitBuf[3*i]  = 1;
			m_pBitBuf[3*i+1]= 1;
			m_pBitBuf[3*i+2]= 0;
		}
		else if( pData[2*i]<0 && pData[2*i+1]<0 && abs(pData[2*i])<abs(pData[2*i+1]))		//7
		{
			m_pBitBuf[3*i]  = 1;
			m_pBitBuf[3*i+1]= 1;
			m_pBitBuf[3*i+2]= 1;
		}
		else if( pData[2*i]>0 && pData[2*i+1]<0 && abs(pData[2*i])<abs(pData[2*i+1]))		//5
		{
			m_pBitBuf[3*i]  = 1;
			m_pBitBuf[3*i+1]= 0;
			m_pBitBuf[3*i+2]= 1;
		}
		else if( pData[2*i]>0 && pData[2*i+1]<0 && abs(pData[2*i])>abs(pData[2*i+1]))		//4
		{
			m_pBitBuf[3*i]  = 1;
			m_pBitBuf[3*i+1]= 0;
			m_pBitBuf[3*i+2]= 0;
		}
		else //( pData[2*i]>0 && pData[2*i+1]>0 && abs(pData[2*i])>abs(pData[2*i+1]) )		//0
		{
			m_pBitBuf[3*i]  = 0;
			m_pBitBuf[3*i+1]= 0;
			m_pBitBuf[3*i+2]= 0;
		}
	}	

	// 差分译码 //
	if (m_bIsDecode)
	{
		CHAR byMod2Sum0,byMod2Sum1,byMod2Sum2;

		CHAR byCode0,byCode1,byCode2;

		////求本次输入第一个点的差分码////
		byMod2Sum0 = (m_PreCode[0] + m_PreCode[1] + m_PreCode[2])%2;
		byMod2Sum1 = (m_PreCode[0] + m_PreCode[1])%2;
		byMod2Sum2 = (m_pBitBuf[2] + m_PreCode[2])%2;

		if( byMod2Sum0 == 0 )
		{
			if(byMod2Sum1 == 0)
			{
				byCode0 = (m_pBitBuf[0] + m_PreCode[0])%2;
				byCode1 = (m_pBitBuf[1] + m_PreCode[1])%2;
				byCode2 = (m_pBitBuf[2] + m_PreCode[2])%2;
			}
			else
			{
				byCode0 = (m_pBitBuf[1] + m_PreCode[1])%2;
				byCode1 = (m_pBitBuf[0] + m_PreCode[0])%2;
				byCode2 = (m_pBitBuf[2] + m_PreCode[2])%2;
			}
		}
		else
		{
			if (byMod2Sum1 == 0)
			{
				if (byMod2Sum2 == 0)
				{
					byCode0 = (m_pBitBuf[0] + m_PreCode[0])%2;
					byCode1 = (byCode0 + m_pBitBuf[2] + m_PreCode[2])%2;
					byCode2 = (byCode0 + m_pBitBuf[1] + m_PreCode[1])%2;
				}
				else
				{
					byCode1 = (m_pBitBuf[1] + m_PreCode[1])%2;
					byCode0 = (byCode1 + m_pBitBuf[2] + m_PreCode[2])%2;
					byCode2 = (byCode1 + m_pBitBuf[0] + m_PreCode[0])%2;
				}
			}
			else
			{
				if (byMod2Sum2 == 0)
				{
					byCode0 = (m_pBitBuf[1] + m_PreCode[1])%2;
					byCode1 = (byCode0 + m_pBitBuf[2] + m_PreCode[2])%2;
					byCode2 = (byCode0 + m_pBitBuf[0] + m_PreCode[0])%2;
				}
				else
				{
					byCode1 = (m_pBitBuf[0] + m_PreCode[0])%2;
					byCode0 = (byCode1 + m_pBitBuf[2] + m_PreCode[2])%2;
					byCode2 = (byCode1 + m_pBitBuf[1] + m_PreCode[1])%2;
				}
			}
		}

		m_pBitBuf[0] = byCode0;
		m_pBitBuf[1] = byCode1;
		m_pBitBuf[2] = byCode2;

		////求差分码////
		for(DWORD i=1;i<dwDataLen/2;i++)
		{
			byMod2Sum0 = (m_pBitBuf[3*i-3] + m_pBitBuf[3*i-2] + m_pBitBuf[3*i-1])%2;
			byMod2Sum1 = (m_pBitBuf[3*i-3] + m_pBitBuf[3*i-2])%2;
			byMod2Sum2 = (m_pBitBuf[3*i+2] + m_pBitBuf[3*i-1])%2;

			if( byMod2Sum0 == 0 )
			{
				if(byMod2Sum1 == 0)
				{
					byCode0 = (m_pBitBuf[3*i] + m_pBitBuf[3*i-3])%2;
					byCode1 = (m_pBitBuf[3*i+1] + m_pBitBuf[3*i-2])%2;
					byCode2 = (m_pBitBuf[3*i+2] + m_pBitBuf[3*i-1])%2;
				}
				else
				{
					byCode0 = (m_pBitBuf[3*i+1] + m_pBitBuf[3*i-2])%2;
					byCode1 = (m_pBitBuf[3*i] + m_pBitBuf[3*i-3])%2;
					byCode2 = (m_pBitBuf[3*i+2] + m_pBitBuf[3*i-1])%2;
				}
			}
			else
			{
				if (byMod2Sum1 == 0)
				{
					if (byMod2Sum2 == 0)
					{
						byCode0 = (m_pBitBuf[3*i] + m_pBitBuf[3*i-3])%2;
						byCode1 = (byCode0 + m_pBitBuf[3*i+2] + m_pBitBuf[3*i-1])%2;
						byCode2 = (byCode0 + m_pBitBuf[3*i+1] + m_pBitBuf[3*i-2])%2;
					}
					else
					{
						byCode1 = (m_pBitBuf[3*i+1] + m_pBitBuf[3*i-2])%2;
						byCode0 = (byCode1 + m_pBitBuf[3*i+2] + m_pBitBuf[3*i-1])%2;
						byCode2 = (byCode1 + m_pBitBuf[3*i] + m_pBitBuf[3*i-3])%2;
					}
				}
				else
				{
					if (byMod2Sum2 == 0)
					{
						byCode0 = (m_pBitBuf[3*i+1] + m_pBitBuf[3*i-2])%2;
						byCode1 = (byCode0 + m_pBitBuf[3*i+2] + m_pBitBuf[3*i-1])%2;
						byCode2 = (byCode0 + m_pBitBuf[3*i] + m_pBitBuf[3*i-3])%2;
					}
					else
					{
						byCode1 = (m_pBitBuf[3*i] + m_pBitBuf[3*i-3])%2;
						byCode0 = (byCode1 + m_pBitBuf[3*i+2] + m_pBitBuf[3*i-1])%2;
						byCode2 = (byCode1 + m_pBitBuf[3*i+1] + m_pBitBuf[3*i-2])%2;
					}
				}
			}

			m_pBitBuf[3*i]  = byCode0;
			m_pBitBuf[3*i+1]= byCode1;
			m_pBitBuf[3*i+2]= byCode2;
		}

		////保存本次输入最后一个点的检测码////
		m_PreCode[0] = m_pBitBuf[m_dwBitNum-3];
		m_PreCode[1] = m_pBitBuf[m_dwBitNum-2];	
		m_PreCode[2] = m_pBitBuf[m_dwBitNum-1];	
	}

	return(dwErrorID);
}

//=================================
// 16PSK信号译码，部分差分解码 //
// ================================
DWORD CSignalDemodProbe::Decode_16PSK( LONG *pData,DWORD dwDataLen )
{
	DWORD dwErrorID = ERROR_SUCCESS;

	if (m_pBitBuf != NULL && m_dwBufLen < 2*dwDataLen)
	{
		delete []m_pBitBuf;

		m_pBitBuf = NULL;
	}	

	if (m_pBitBuf == NULL)
	{
		m_dwBufLen = 2*dwDataLen;

		m_pBitBuf = new CHAR[m_dwBufLen];
	}

	m_dwBitNum = 2*dwDataLen;
    ZeroMemory(m_pBitBuf,m_dwBufLen*sizeof(CHAR));

	// 格雷码逻辑映射
	for (DWORD i = 0; i < dwDataLen/2; i++)
	{		
		if(pData[2*i]>0 && pData[2*i+1]>0)			//第一象限
		{
			m_pBitBuf[4*i]  =0;
			m_pBitBuf[4*i+1]=0;
		}
		else if(pData[2*i]<0 && pData[2*i+1]>0)		//第二象限
		{
			m_pBitBuf[4*i]  =0;
			m_pBitBuf[4*i+1]=1;
		}
		else if(pData[2*i]<0 && pData[2*i+1]<0)		//第三象限
		{
			m_pBitBuf[4*i]  =1;
			m_pBitBuf[4*i+1]=1;
		}
		else //if(pData[2*i]>0 && pData[2*i+1]<0)	//第四象限
		{
			m_pBitBuf[4*i]  =1;
			m_pBitBuf[4*i+1]=0;
		}

		if (abs(pData[2*i]) > 0)
		{
			if( abs(pData[2*i]) > abs(pData[2*i+1]) &&  abs(pData[2*i+1])/abs(pData[2*i]) < 0.414213562373)  
			{
				m_pBitBuf[4*i+2]=0;
				m_pBitBuf[4*i+3]=0;
			}
			else if( abs(pData[2*i]) > abs(pData[2*i+1]) &&  abs(pData[2*i+1])/abs(pData[2*i]) > 0.414213562373)
			{
				m_pBitBuf[4*i+2]=0;
				m_pBitBuf[4*i+3]=1;
			}
			else if( abs(pData[2*i]) < abs(pData[2*i+1]) &&  abs(pData[2*i+1])/abs(pData[2*i]) < 2.414213562373 )
			{
				m_pBitBuf[4*i+2]=1;
				m_pBitBuf[4*i+3]=1;
			}
			else //if( abs(pData[2*i]) < abs(pData[2*i+1]) &&  abs(pData[2*i+1])/abs(pData[2*i]) > 2.414213562373 )
			{
				m_pBitBuf[4*i+2]=1;
				m_pBitBuf[4*i+3]=0;
			}
		}
		else
		{
			m_pBitBuf[4*i+2]=0;
			m_pBitBuf[4*i+3]=0;
		}		
	}	

	return(dwErrorID);
}

//=================================
// 16APSK信号译码，部分差分解码 //
// ================================
DWORD CSignalDemodProbe::Decode_16APSK( LONG *pData,DWORD dwDataLen )
{
	DWORD i = 0;

	DWORD dwErrorID = ERROR_SUCCESS;	

	if (m_pSgnlBuf != NULL && m_dwSgnlLen < dwDataLen)
	{
		delete []m_pSgnlBuf;

		m_pSgnlBuf = NULL;
	}	

	if (m_pSgnlBuf == NULL)
	{
		m_dwSgnlLen = dwDataLen;

		m_pSgnlBuf = new double[m_dwSgnlLen];
	}

	if (m_pBitBuf != NULL && m_dwBufLen < 2*dwDataLen)
	{
		delete []m_pBitBuf;

		m_pBitBuf = NULL;
	}	

	if (m_pBitBuf == NULL)
	{
		m_dwBufLen = 2*dwDataLen;

		m_pBitBuf = new CHAR[m_dwBufLen];
	}

	m_dwBitNum = 2*dwDataLen;
	ZeroMemory(m_pBitBuf,m_dwBufLen*sizeof(CHAR));

	///////////////////////////////////////
	////求数据归一化因子///	
	double dMeanValueI=0.0;
	double dMeanValueQ=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dMeanValueI += pData[2*i]*pData[2*i];
		dMeanValueQ += pData[2*i+1]*pData[2*i+1];			
	}
	dMeanValueI /= dwDataLen/2;
	dMeanValueQ /= dwDataLen/2;

	if (dMeanValueI < 1e-5 || dMeanValueQ < 1e-5)
	{
		return(dwErrorID);
	}

	////数据归一化///
	for(i=0;i<dwDataLen/2;i++)
	{
		m_pSgnlBuf[2*i] = pData[2*i]*sqrt(5.0)/sqrt(dMeanValueI);
		m_pSgnlBuf[2*i+1] = pData[2*i+1]*sqrt(5.0)/sqrt(dMeanValueQ);
	}

	// 格雷码逻辑映射 //
	for (i = 0; i < dwDataLen/2; i++)
	{		
		if(m_pSgnlBuf[2*i]>0 && m_pSgnlBuf[2*i+1]>0)			//第一象限
		{
			m_pBitBuf[4*i]  =0;
			m_pBitBuf[4*i+1]=0;
		}
		else if(m_pSgnlBuf[2*i]<0 && m_pSgnlBuf[2*i+1]>0)		//第二象限
		{
			m_pBitBuf[4*i]  =0;
			m_pBitBuf[4*i+1]=1;
		}
		else if(m_pSgnlBuf[2*i]<0 && m_pSgnlBuf[2*i+1]<0)		//第三象限
		{
			m_pBitBuf[4*i]  =1;
			m_pBitBuf[4*i+1]=1;
		}
		else //if(m_pSgnlBuf[2*i]>0 && m_pSgnlBuf[2*i+1]<0)	//第四象限
		{
			m_pBitBuf[4*i]  =1;
			m_pBitBuf[4*i+1]=0;
		}

		if( fabs(m_pSgnlBuf[2*i]) < 2 && fabs(m_pSgnlBuf[2*i+1]) < 2 )  // 1
		{
			m_pBitBuf[4*i+2]=1;
			m_pBitBuf[4*i+3]=1;
		}
		else if( fabs(m_pSgnlBuf[2*i])<2 && fabs(m_pSgnlBuf[2*i+1]) > 2 )  //[1,3],[-1,-3]
		{
			m_pBitBuf[4*i+2]=1;
			m_pBitBuf[4*i+3]=0;
		}
		else if( fabs(m_pSgnlBuf[2*i]) > 2 && fabs(m_pSgnlBuf[2*i+1]) < 2 )	//[3,-1],[-3,1]
		{
			m_pBitBuf[4*i+2]=0;
			m_pBitBuf[4*i+3]=1;
		}
		else // if( fabs(m_pSgnlBuf[2*i]) > 2 && fabs(m_pSgnlBuf[2*i+1]) > 2 )	// 3
		{
			m_pBitBuf[4*i+2]=0;
			m_pBitBuf[4*i+3]=0;
		}		
	}	

	// 差分译码 //
	if (m_bIsDecode)
	{
		// 部分差分 //
		CHAR byMod2Sum,byCode0,byCode1;

		////求本次输入第一个点的差分码////
		byMod2Sum = (m_pBitBuf[0]+m_pBitBuf[1]+m_PreCode[0]+m_PreCode[1])%2;

		if( byMod2Sum == 0 )
		{
			byCode0 = (m_pBitBuf[0] + m_PreCode[0])%2;
			byCode1 = (m_pBitBuf[1] + m_PreCode[1])%2;
		}
		else
		{
			byCode0 =(m_pBitBuf[0] + m_PreCode[1])%2;
			byCode1 =(m_pBitBuf[1] + m_PreCode[0])%2;
		}

		m_pBitBuf[0] = byCode0;
		m_pBitBuf[1] = byCode1;

		////求差分码////
		for(i=1;i<dwDataLen/2;i++)
		{
			byMod2Sum = (m_pBitBuf[4*i] + m_pBitBuf[4*i+1] + m_pBitBuf[4*(i-1)]
			+ m_pBitBuf[4*(i-1)+1])%2;

			if( byMod2Sum == 0 )
			{
				byCode0 = (m_pBitBuf[4*i] + m_pBitBuf[4*(i-1)])%2;
				byCode1 = (m_pBitBuf[4*i+1] + m_pBitBuf[4*(i-1)+1])%2;
			}
			else
			{
				byCode0 = (m_pBitBuf[4*i] + m_pBitBuf[4*(i-1)+1])%2;
				byCode1 = (m_pBitBuf[4*i+1] + m_pBitBuf[4*(i-1)])%2;
			}

			m_pBitBuf[4*i]  = byCode0;
			m_pBitBuf[4*i+1]= byCode1;
		}

		////保存本次输入最后一个点的检测码////
		m_PreCode[0] = m_pBitBuf[m_dwBitNum-4];
		m_PreCode[1] = m_pBitBuf[m_dwBitNum-3];
	}		

	return(dwErrorID);
}

// 16QAM信号译码，部分差分解码 //
DWORD CSignalDemodProbe::Decode_16QAM(LONG *pData, DWORD dwDataLen)
{
	DWORD i = 0;

	DWORD dwErrorID = ERROR_SUCCESS;	

	if (m_pSgnlBuf != NULL && m_dwSgnlLen < dwDataLen)
	{
		delete []m_pSgnlBuf;

		m_pSgnlBuf = NULL;
	}	

	if (m_pSgnlBuf == NULL)
	{
		m_dwSgnlLen = dwDataLen;

		m_pSgnlBuf = new double[m_dwSgnlLen];
	}

	if (m_pBitBuf != NULL && m_dwBufLen < 2*dwDataLen)
	{
		delete []m_pBitBuf;

		m_pBitBuf = NULL;
	}	

	if (m_pBitBuf == NULL)
	{
		m_dwBufLen = 2*dwDataLen;

		m_pBitBuf = new CHAR[m_dwBufLen];
	}

	m_dwBitNum = 2*dwDataLen;
	ZeroMemory(m_pBitBuf,m_dwBufLen*sizeof(CHAR));

	///////////////////////////////////////
	////求数据归一化因子///	
	double dMeanValueI=0.0;
	double dMeanValueQ=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dMeanValueI += pData[2*i]*pData[2*i];
		dMeanValueQ += pData[2*i+1]*pData[2*i+1];			
	}
	dMeanValueI /= dwDataLen/2;
	dMeanValueQ /= dwDataLen/2;

	if (dMeanValueI < 1e-5 || dMeanValueQ < 1e-5)
	{
		return(dwErrorID);
	}

	////数据归一化///
	for(i=0;i<dwDataLen/2;i++)
	{
		m_pSgnlBuf[2*i] = pData[2*i]*sqrt(5.0)/sqrt(dMeanValueI);
		m_pSgnlBuf[2*i+1] = pData[2*i+1]*sqrt(5.0)/sqrt(dMeanValueQ);
	}

	// 格雷码逻辑映射 //
	for (i = 0; i < dwDataLen/2; i++)
	{		
		if(m_pSgnlBuf[2*i]>0 && m_pSgnlBuf[2*i+1]>0)			//第一象限
		{
			m_pBitBuf[4*i]  =0;
			m_pBitBuf[4*i+1]=0;
		}
		else if(m_pSgnlBuf[2*i]<0 && m_pSgnlBuf[2*i+1]>0)		//第二象限
		{
			m_pBitBuf[4*i]  =0;
			m_pBitBuf[4*i+1]=1;
		}
		else if(m_pSgnlBuf[2*i]<0 && m_pSgnlBuf[2*i+1]<0)		//第三象限
		{
			m_pBitBuf[4*i]  =1;
			m_pBitBuf[4*i+1]=1;
		}
		else //if(m_pSgnlBuf[2*i]>0 && m_pSgnlBuf[2*i+1]<0)	//第四象限
		{
			m_pBitBuf[4*i]  =1;
			m_pBitBuf[4*i+1]=0;
		}

		if( fabs(m_pSgnlBuf[2*i]) > 2 && fabs(m_pSgnlBuf[2*i+1]) > 2 )  // 1
		{
			m_pBitBuf[4*i+2]=1;
			m_pBitBuf[4*i+3]=1;
		}
		else if( fabs(m_pSgnlBuf[2*i])<2 && fabs(m_pSgnlBuf[2*i+1]) > 2 &&
			m_pSgnlBuf[2*i]*m_pSgnlBuf[2*i+1] > 0 )  //[1,3],[-1,-3]
		{
			m_pBitBuf[4*i+2]=1;
			m_pBitBuf[4*i+3]=0;
		}
		else if( fabs(m_pSgnlBuf[2*i]) > 2 && fabs(m_pSgnlBuf[2*i+1]) < 2 && 
			m_pSgnlBuf[2*i]*m_pSgnlBuf[2*i+1]<0 )	//[3,-1],[-3,1]
		{
			m_pBitBuf[4*i+2]=1;
			m_pBitBuf[4*i+3]=0;
		}
		else if( fabs(m_pSgnlBuf[2*i]) < 2 && fabs(m_pSgnlBuf[2*i+1]) < 2 )	// 3
		{
			m_pBitBuf[4*i+2]=0;
			m_pBitBuf[4*i+3]=0;
		}
		else if( fabs(m_pSgnlBuf[2*i])<2 && fabs(m_pSgnlBuf[2*i+1]) > 2 && 
			m_pSgnlBuf[2*i]*m_pSgnlBuf[2*i+1]<0 )	//[-1,3],[1,-3]
		{
			m_pBitBuf[4*i+2]=0;
			m_pBitBuf[4*i+3]=1;
		}
		else //if( fabs(m_pSgnlBuf[2*i])>2 && fabs(m_pSgnlBuf[2*i+1]) < 2 && 
			//m_pSgnlBuf[2*i]*m_pSgnlBuf[2*i+1]>0 )	//[3,1],[-3,-1]
		{
			m_pBitBuf[4*i+2]=0;
			m_pBitBuf[4*i+3]=1;
		}
	}	

	// 差分译码 //
	if (m_bIsDecode)
	{
		// 部分差分 //
		CHAR byMod2Sum,byCode0,byCode1;

		////求本次输入第一个点的差分码////
		byMod2Sum = (m_pBitBuf[0]+m_pBitBuf[1]+m_PreCode[0]+m_PreCode[1])%2;

		if( byMod2Sum == 0 )
		{
			byCode0 = (m_pBitBuf[0] + m_PreCode[0])%2;
			byCode1 = (m_pBitBuf[1] + m_PreCode[1])%2;
		}
		else
		{
			byCode0 =(m_pBitBuf[0] + m_PreCode[1])%2;
			byCode1 =(m_pBitBuf[1] + m_PreCode[0])%2;
		}

		m_pBitBuf[0] = byCode0;
		m_pBitBuf[1] = byCode1;

		////求差分码////
		for(i=1;i<dwDataLen/2;i++)
		{
			byMod2Sum = (m_pBitBuf[4*i] + m_pBitBuf[4*i+1] + m_pBitBuf[4*(i-1)]
			+ m_pBitBuf[4*(i-1)+1])%2;

			if( byMod2Sum == 0 )
			{
				byCode0 = (m_pBitBuf[4*i] + m_pBitBuf[4*(i-1)])%2;
				byCode1 = (m_pBitBuf[4*i+1] + m_pBitBuf[4*(i-1)+1])%2;
			}
			else
			{
				byCode0 = (m_pBitBuf[4*i] + m_pBitBuf[4*(i-1)+1])%2;
				byCode1 = (m_pBitBuf[4*i+1] + m_pBitBuf[4*(i-1)])%2;
			}

			m_pBitBuf[4*i]  = byCode0;
			m_pBitBuf[4*i+1]= byCode1;
		}

		////保存本次输入最后一个点的检测码////
		m_PreCode[0] = m_pBitBuf[m_dwBitNum-4];
		m_PreCode[1] = m_pBitBuf[m_dwBitNum-3];
	}		

	return(dwErrorID);
}

// Phase Differential Coding //
DWORD CSignalDemodProbe::Decode_32QAM( LONG *pData,DWORD dwDataLen )
{
	DWORD i = 0;

	DWORD dwErrorID = ERROR_SUCCESS;	

	if (m_pSgnlBuf != NULL && m_dwSgnlLen < dwDataLen)
	{
		delete []m_pSgnlBuf;

		m_pSgnlBuf = NULL;
	}	

	if (m_pSgnlBuf == NULL)
	{
		m_dwSgnlLen = dwDataLen;

		m_pSgnlBuf = new double[m_dwSgnlLen];
	}

	if (m_pBitBuf != NULL && m_dwBufLen < 5*dwDataLen/2)
	{
		delete []m_pBitBuf;

		m_pBitBuf = NULL;
	}	

	if (m_pBitBuf == NULL)
	{
		m_dwBufLen = 5*dwDataLen/2;

		m_pBitBuf = new CHAR[m_dwBufLen];
	}

	m_dwBitNum = 5*dwDataLen/2;
	ZeroMemory(m_pBitBuf,m_dwBufLen*sizeof(CHAR));

	///////////////////////////////////////
	////求数据归一化因子///	
	double dMeanValueI=0.0;
	double dMeanValueQ=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dMeanValueI += pData[2*i]*pData[2*i];
		dMeanValueQ += pData[2*i+1]*pData[2*i+1];			
	}
	dMeanValueI /= dwDataLen/2;
	dMeanValueQ /= dwDataLen/2;

	if (dMeanValueI < 1e-5 || dMeanValueQ < 1e-5)
	{
		return(dwErrorID);
	}

	////数据归一化///
	for(i=0;i<dwDataLen/2;i++)
	{
		m_pSgnlBuf[2*i] = pData[2*i]*sqrt(35.0/3)/sqrt(dMeanValueI);
		m_pSgnlBuf[2*i+1] = pData[2*i+1]*sqrt(35.0/3)/sqrt(dMeanValueQ);
	}

	// 格雷码逻辑映射 //
	for (i = 0; i < dwDataLen/2; i++)
	{		
		if(m_pSgnlBuf[2*i]>0 && m_pSgnlBuf[2*i+1]>0)			//第一象限
		{
			m_pBitBuf[5*i]  =0;
			m_pBitBuf[5*i+1]=0;
		}
		else if(m_pSgnlBuf[2*i]<0 && m_pSgnlBuf[2*i+1]>0)		//第二象限
		{
			m_pBitBuf[5*i]  =0;
			m_pBitBuf[5*i+1]=1;
		}
		else if(m_pSgnlBuf[2*i]<0 && m_pSgnlBuf[2*i+1]<0)		//第三象限
		{
			m_pBitBuf[5*i]  =1;
			m_pBitBuf[5*i+1]=1;
		}
		else //if(m_pSgnlBuf[2*i]>0 && m_pSgnlBuf[2*i+1]<0)	//第四象限
		{
			m_pBitBuf[5*i]  =1;
			m_pBitBuf[5*i+1]=0;
		}

		if( fabs(m_pSgnlBuf[2*i]) < 2 && fabs(m_pSgnlBuf[2*i+1]) < 2)  // 1
		{
			m_pBitBuf[5*i+2]=0;
			m_pBitBuf[5*i+3]=0;
			m_pBitBuf[5*i+4]=1;
		}
		else if( fabs(m_pSgnlBuf[2*i]) < 2 && fabs(m_pSgnlBuf[2*i+1]) > 2 && // 5
			fabs(m_pSgnlBuf[2*i+1]) < 4 )  
		{
			m_pBitBuf[5*i+2]=1;
			m_pBitBuf[5*i+3]=0;
			m_pBitBuf[5*i+4]=1;
		}
		else if( fabs(m_pSgnlBuf[2*i]) < 2 && fabs(m_pSgnlBuf[2*i+1]) > 4 )	// 4
		{
			m_pBitBuf[5*i+2]=1;
			m_pBitBuf[5*i+3]=0;
			m_pBitBuf[5*i+4]=0;
		}
		else if( fabs(m_pSgnlBuf[2*i]) > 2 && fabs(m_pSgnlBuf[2*i]) < 4 &&
			fabs(m_pSgnlBuf[2*i+1]) < 2 )	// 2
		{
			m_pBitBuf[5*i+2]=0;
			m_pBitBuf[5*i+3]=1;
			m_pBitBuf[5*i+4]=0;
		}
		else if( fabs(m_pSgnlBuf[2*i]) > 2 && fabs(m_pSgnlBuf[2*i]) < 4 &&
			fabs(m_pSgnlBuf[2*i+1]) > 2 && fabs(m_pSgnlBuf[2*i+1]) < 4 )	//6
		{
			m_pBitBuf[5*i+2]=1;
			m_pBitBuf[5*i+3]=1;
			m_pBitBuf[5*i+4]=0;
		}
		else if( fabs(m_pSgnlBuf[2*i]) > 2 && fabs(m_pSgnlBuf[2*i]) < 4 &&
			fabs(m_pSgnlBuf[2*i+1]) > 4 )	//0
		{
			m_pBitBuf[5*i+2]=0;
			m_pBitBuf[5*i+3]=0;
			m_pBitBuf[5*i+4]=0;
		}
		else if(  fabs(m_pSgnlBuf[2*i]) > 4 && fabs(m_pSgnlBuf[2*i+1]) < 2 )	//3
		{
			m_pBitBuf[5*i+2]=0;
			m_pBitBuf[5*i+3]=1;
			m_pBitBuf[5*i+4]=1;
		}
		else //if(  fabs(m_pSgnlBuf[2*i]) > 4 && fabs(m_pSgnlBuf[2*i+1]) > 2  && fabs(m_pSgnlBuf[2*i+1]) < 4)	//7
		{
			m_pBitBuf[5*i+2]=1;
			m_pBitBuf[5*i+3]=1;
			m_pBitBuf[5*i+4]=1;
		}
	}

	// 差分译码 //
	if (m_bIsDecode)
	{
		// 部分差分 //
		CHAR byMod2Sum,byCode0,byCode1;

		////求本次输入第一个点的差分码////
		byMod2Sum = (m_pBitBuf[0]+m_pBitBuf[1]+m_PreCode[0]+m_PreCode[1])%2;

		if( byMod2Sum == 0 )
		{
			byCode0 = (m_pBitBuf[0] + m_PreCode[0])%2;
			byCode1 = (m_pBitBuf[1] + m_PreCode[1])%2;
		}
		else
		{
			byCode0 =(m_pBitBuf[0] + m_PreCode[1])%2;
			byCode1 =(m_pBitBuf[1] + m_PreCode[0])%2;
		}

		m_pBitBuf[0] = byCode0;
		m_pBitBuf[1] = byCode1;

		////求差分码////
		for(i=1;i<dwDataLen/2;i++)
		{
			byMod2Sum = (m_pBitBuf[5*i] + m_pBitBuf[5*i+1] + m_pBitBuf[5*(i-1)]
			+ m_pBitBuf[5*(i-1)+1])%2;

			if( byMod2Sum == 0 )
			{
				byCode0 = (m_pBitBuf[5*i] + m_pBitBuf[5*(i-1)])%2;
				byCode1 = (m_pBitBuf[5*i+1] + m_pBitBuf[5*(i-1)+1])%2;
			}
			else
			{
				byCode0 = (m_pBitBuf[5*i] + m_pBitBuf[5*(i-1)+1])%2;
				byCode1 = (m_pBitBuf[5*i+1] + m_pBitBuf[5*(i-1)])%2;
			}

			m_pBitBuf[5*i]  = byCode0;
			m_pBitBuf[5*i+1]= byCode1;
		}

		////保存本次输入最后一个点的检测码////
		m_PreCode[0] = m_pBitBuf[m_dwBitNum-5];
		m_PreCode[1] = m_pBitBuf[m_dwBitNum-4];
	}	

	return(dwErrorID);
}

DWORD CSignalDemodProbe::Decode_64QAM( LONG *pData,DWORD dwDataLen )
{
	DWORD i = 0;

	DWORD dwErrorID = ERROR_SUCCESS;	

	if (m_pSgnlBuf != NULL && m_dwSgnlLen < dwDataLen)
	{
		delete []m_pSgnlBuf;

		m_pSgnlBuf = NULL;
	}	

	if (m_pSgnlBuf == NULL)
	{
		m_dwSgnlLen = dwDataLen;

		m_pSgnlBuf = new double[m_dwSgnlLen];
	}

	if (m_pBitBuf != NULL && m_dwBufLen < 3*dwDataLen)
	{
		delete []m_pBitBuf;

		m_pBitBuf = NULL;
	}	

	if (m_pBitBuf == NULL)
	{
		m_dwBufLen = 3*dwDataLen;

		m_pBitBuf = new CHAR[m_dwBufLen];
	}

	m_dwBitNum = 3*dwDataLen;
	ZeroMemory(m_pBitBuf,m_dwBufLen*sizeof(CHAR));

	///////////////////////////////////////
	////求数据归一化因子///	
	double dMeanValueI=0.0;
	double dMeanValueQ=0.0;
	for(i=0;i<dwDataLen/2;i++)
	{
		dMeanValueI += pData[2*i]*pData[2*i];
		dMeanValueQ += pData[2*i+1]*pData[2*i+1];			
	}
	dMeanValueI /= dwDataLen/2;
	dMeanValueQ /= dwDataLen/2;

	if (dMeanValueI < 1e-5 || dMeanValueQ < 1e-5)
	{
		return(dwErrorID);
	}

	////数据归一化///
	for(i=0;i<dwDataLen/2;i++)
	{
		m_pSgnlBuf[2*i] = pData[2*i]*sqrt(21.0)/sqrt(dMeanValueI);
		m_pSgnlBuf[2*i+1] = pData[2*i+1]*sqrt(21.0)/sqrt(dMeanValueQ);
	}

	// 格雷码逻辑映射 //
	for (i = 0; i < dwDataLen/2; i++)
	{		
		if(m_pSgnlBuf[2*i]>0 && m_pSgnlBuf[2*i+1]>0)			//第一象限
		{
			m_pBitBuf[6*i]  =0;
			m_pBitBuf[6*i+1]=0;
		}
		else if(m_pSgnlBuf[2*i]<0 && m_pSgnlBuf[2*i+1]>0)		//第二象限
		{
			m_pBitBuf[6*i]  =1;
			m_pBitBuf[6*i+1]=0;
		}
		else if(m_pSgnlBuf[2*i]<0 && m_pSgnlBuf[2*i+1]<0)		//第三象限
		{
			m_pBitBuf[6*i]  =1;
			m_pBitBuf[6*i+1]=1;
		}
		else if(m_pSgnlBuf[2*i]>0 && m_pSgnlBuf[2*i+1]<0)		//第四象限
		{
			m_pBitBuf[6*i]  =0;
			m_pBitBuf[6*i+1]=1;
		}

		if(m_pSgnlBuf[2*i] - 4 > 0 && m_pSgnlBuf[2*i+1] - 4 > 0)		//
		{
			m_pBitBuf[6*i+2]=1;
			m_pBitBuf[6*i+3]=1;
		}
		else if(m_pSgnlBuf[2*i] - 4 < 0 && m_pSgnlBuf[2*i+1] - 4 > 0 )	//
		{
			m_pBitBuf[6*i+2]=1;
			m_pBitBuf[6*i+3]=0;
		}
		else if(m_pSgnlBuf[2*i] - 4 < 0 && m_pSgnlBuf[2*i+1] - 4 < 0 )	//
		{
			m_pBitBuf[6*i+2]=0;
			m_pBitBuf[6*i+3]=0;
		}
		else if( m_pSgnlBuf[2*i] - 4 > 0 && m_pSgnlBuf[2*i+1]- 4 < 0 )	//
		{
			m_pBitBuf[6*i+2]=0;
			m_pBitBuf[6*i+3]=1;
		}

		if(fabs(m_pSgnlBuf[2*i] - 4) < 2 && fabs(m_pSgnlBuf[2*i+1] - 4) < 2)		//
		{
			m_pBitBuf[6*i+4]=1;
			m_pBitBuf[6*i+5]=1;
		}
		else if( fabs(m_pSgnlBuf[2*i] - 4) > 2 && fabs(m_pSgnlBuf[2*i+1] - 4) < 2)	//
		{
			m_pBitBuf[6*i+4]=1;
			m_pBitBuf[6*i+5]=0;
		}
		else if( fabs(m_pSgnlBuf[2*i] - 4) > 2 && fabs(m_pSgnlBuf[2*i+1]- 4) > 2)	//
		{
			m_pBitBuf[6*i+4]=0;
			m_pBitBuf[6*i+5]=0;
		}
		else //if( fabs(m_pSgnlBuf[2*i] - 4) < 2 && fabs(m_pSgnlBuf[2*i+1] - 4) > 2)	//
		{
			m_pBitBuf[6*i+4]=0;
			m_pBitBuf[6*i+5]=1;
		}		
	}

	// 差分译码 //
	if (m_bIsDecode)
	{
		// 部分差分 //
		CHAR byMod2Sum,byCode0,byCode1;

		////求本次输入第一个点的差分码////
		byMod2Sum = (m_pBitBuf[0]+m_pBitBuf[1]+m_PreCode[0]+m_PreCode[1])%2;

		if( byMod2Sum == 0 )
		{
			byCode0 = (m_pBitBuf[0] + m_PreCode[0])%2;
			byCode1 = (m_pBitBuf[1] + m_PreCode[1])%2;
		}
		else
		{
			byCode0 =(m_pBitBuf[0] + m_PreCode[1])%2;
			byCode1 =(m_pBitBuf[1] + m_PreCode[0])%2;
		}

		m_pBitBuf[0] = byCode0;
		m_pBitBuf[1] = byCode1;

		////求差分码////
		for(i=1;i<dwDataLen/2;i++)
		{
			byMod2Sum = (m_pBitBuf[6*i] + m_pBitBuf[6*i+1] + m_pBitBuf[6*(i-1)]
			+ m_pBitBuf[6*(i-1)+1])%2;

			if( byMod2Sum == 0 )
			{
				byCode0 = (m_pBitBuf[6*i] + m_pBitBuf[6*(i-1)])%2;
				byCode1 = (m_pBitBuf[6*i+1] + m_pBitBuf[6*(i-1)+1])%2;
			}
			else
			{
				byCode0 = (m_pBitBuf[6*i] + m_pBitBuf[6*(i-1)+1])%2;
				byCode1 = (m_pBitBuf[6*i+1] + m_pBitBuf[6*(i-1)])%2;
			}

			m_pBitBuf[6*i]  = byCode0;
			m_pBitBuf[6*i+1]= byCode1;
		}

		////保存本次输入最后一个点的检测码////
		m_PreCode[0] = m_pBitBuf[m_dwBitNum-5];
		m_PreCode[1] = m_pBitBuf[m_dwBitNum-4];
	}	

	return(dwErrorID);
}


