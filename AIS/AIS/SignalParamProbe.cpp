#include "stdafx.h"
#include "SignalParamProbe.h"
#include "SgnlPrcsDll.h"



int FREQ_MAX_LEN = 200;

CSignalParamProbe::CSignalParamProbe(void)
{
	m_dSymbolRate = 0.0;
	m_dSignalFreq = 0.0;
	m_dFreqShift  = 0.0;

	m_dSamplingFreq = 0.0;	
	m_dPasswidth    = 0.0;
	m_nModuType = SMT_UNKNOWN;

	m_sap.dAmplAveSqu = .0;
	m_sap.dCenterFreq = .0;
	m_sap.dDASPeakLvl = 0.0;
	m_sap.dSpecPeakLvl= 0.0;
	m_sap.nDASPeakIdx = 0;
	m_sap.nPeakIndex1 = 0;
	m_sap.nPeakIndex2 = 0;
	m_sap.nBandwidth= 0;
	m_sap.nSpecPeakNum= 0;
	m_sap.nSpecType   = 0;
	m_sap.nSquaPeakIdx1= 0;
	m_sap.nSquaPeakIdx2= 0;
	m_sap.dAMForOrder = 0;
	m_sap.dFMForOrder = 0;
	m_sap.dFMStatWight = 0;
	m_sap.dAMStatWight = 0;

	m_sap.dPsdLvlBnd = 0;
	m_sap.dPsdPeakDeT = 0;
	m_sap.dPsdCentFre  = 0;
	m_sap.dPsdSymmet  = 0;
	m_sap.nPsdPeakIdx1 = 0;
	m_sap.nPsdPeakIdx2 = 0;
	m_sap.nPsdPeakNum = 0;	

	m_dwFFTSize = 0;
	m_dwFFTMove = 0;
	m_nFFTAver = 0;
	m_nFFTWind = 0;
	m_dwSignLen = 0;
	m_dwIQLen	= 0;   

	m_pDataBuff = NULL;
	m_pSignalBuff = NULL;

	m_pTmpBufI = NULL;
	m_pTmpBufQ = NULL;

	m_pAmplBuff = NULL;
	m_pFreqBuff = NULL;

	m_pSignPsd = NULL;
	m_pAmplPSD = NULL;

	m_pBuffer1 = NULL;
	m_pBuffer2 = NULL;
	m_pBuffer3 = NULL;

	m_pPsdInex = NULL;

	m_pFFTIQ = NULL;
	m_pFFTAD = NULL;
}

CSignalParamProbe::~CSignalParamProbe(void)
{
	if (m_pDataBuff != NULL)
	{
		delete []m_pDataBuff;

		m_pDataBuff = NULL;
	}
	if (NULL != m_pSignPsd)
	{
		delete []m_pSignPsd;

		delete []m_pPsdInex;

		delete []m_pAmplPSD;

		delete []m_pBuffer1;
		delete []m_pBuffer2;
		delete []m_pBuffer3;
	}

	if (NULL != m_pSignalBuff)
	{
		delete []m_pTmpBufI;
		delete []m_pTmpBufQ;

		delete []m_pAmplBuff;	
		delete []m_pFreqBuff;

		delete []m_pSignalBuff;		
	}

	if (m_pFFTIQ != NULL)
	{
		delete m_pFFTIQ;

		m_pFFTIQ = NULL;
	}

	if (m_pFFTAD != NULL)
	{
		delete m_pFFTAD;

		m_pFFTAD = NULL;
	}
}

DWORD CSignalParamProbe::InitParamProbe(double dSamplingFreq, 
										double dPasswidth,
										DWORD dwFFTSize, 
										SHORT nModuType /*= SMT_UNKNOWN*/)
{
	// Incorrect Sampling Frequency
	if (dSamplingFreq<0.000001)
	{
		return ERROR_UD_INCORRECT_DDC_PARAMETER;
	}

	// Undefined Signal Modulation Type
	if ((nModuType < SMT_UNKNOWN) || (nModuType > SMT_UHF_LINK4A))
	{
		return(ERROR_UNDEFINED_MODULATION_TYPE);
	}
	else
	{
		m_nModuType = nModuType;
	}

	m_dSamplingFreq = dSamplingFreq;
	m_dPasswidth = dPasswidth;
	m_dwFFTSize = dwFFTSize ;
	m_dwFFTSize = m_dwFFTSize < 1024 ? 1024:m_dwFFTSize;
	m_dwFFTSize = m_dwFFTSize > 16384 ? 16384:m_dwFFTSize;
	m_dwFFTMove = dwFFTSize/2;
	m_nFFTAver = 8;
	m_nFFTWind = Blackman;

	// 申请资源	
	m_pAmplPSD = new double[m_dwFFTSize];
	m_pSignPsd = new double[m_dwFFTSize];

	ZeroMemory(m_pAmplPSD,m_dwFFTSize*sizeof(double));
	ZeroMemory(m_pSignPsd,m_dwFFTSize*sizeof(double));

	m_pBuffer1 = new double[m_dwFFTSize];
	m_pBuffer2 = new double[m_dwFFTSize];
	m_pBuffer3 = new double[m_dwFFTSize];	

	ZeroMemory(m_pBuffer1,m_dwFFTSize*sizeof(double));
	ZeroMemory(m_pBuffer2,m_dwFFTSize*sizeof(double));
	ZeroMemory(m_pBuffer3,m_dwFFTSize*sizeof(double));

	m_pPsdInex = new LONG[m_dwFFTSize];
	ZeroMemory(m_pPsdInex,m_dwFFTSize*sizeof(LONG));
	
	// 信号长度
	m_dwSignLen = 2*((m_nFFTAver-1)*m_dwFFTMove + m_dwFFTSize);

	m_pDataBuff = new double[m_dwSignLen];
	m_pSignalBuff = new double[m_dwSignLen];
	m_pAmplBuff = new double[m_dwSignLen/2];
	m_pFreqBuff = new double[m_dwSignLen/2];

	ZeroMemory(m_pDataBuff,m_dwSignLen*sizeof(double));
	ZeroMemory(m_pSignalBuff,m_dwSignLen*sizeof(double));
	ZeroMemory(m_pAmplBuff,m_dwSignLen*sizeof(double)/2);
	ZeroMemory(m_pFreqBuff,m_dwSignLen*sizeof(double)/2);

	m_dwTmpBufLen = m_dwSignLen > m_dwFFTSize ? m_dwSignLen : m_dwFFTSize;

	m_pTmpBufI = new double[m_dwTmpBufLen];	
	m_pTmpBufQ = new double[m_dwTmpBufLen];		
	ZeroMemory(m_pTmpBufI,m_dwTmpBufLen*sizeof(double));
	ZeroMemory(m_pTmpBufQ,m_dwTmpBufLen*sizeof(double));	

	m_dwIQLen = 0;

	m_pFFTIQ = new CSpectrumProbe;
	m_pFFTIQ->InitSpectrumProbeParam(m_dwFFTSize,m_dwFFTMove,m_nFFTAver,m_nFFTWind,true,false,false);

	SHORT nFFTAver = SHORT((m_dwSignLen/2-2*m_dwFFTSize)/m_dwFFTSize + 1);
	
	m_pFFTAD = new CSpectrumProbe;
	m_pFFTAD->InitSpectrumProbeParam(2*m_dwFFTSize,m_dwFFTSize, nFFTAver,m_nFFTWind,false,false,false);

	return(ERROR_SUCCESS);
}

DWORD CSignalParamProbe::InputData(double *pData, DWORD dwDataLen)
{
	DWORD dwErrorID = ERROR_SUCCESS;	

	DWORD dwIQLen = 0;

	if (m_dwSignLen <= dwDataLen)
	{
		dwIQLen = m_dwSignLen;
	}
	else
	{
		dwIQLen = m_dwSignLen - m_dwIQLen;

		dwIQLen = (dwIQLen > dwDataLen) ? dwDataLen:dwIQLen;
	}

    memmove(m_pDataBuff + m_dwIQLen,pData,dwIQLen*sizeof(double));

	m_dwIQLen += dwDataLen;

	return(dwErrorID);
}

DWORD CSignalParamProbe::GetSignalParam(SIGNAL_DEMOD_PARAM& pSignalPara)
{
	// 分析数据不足 //
	if (m_dwIQLen < m_dwSignLen)
	{
		return(ERROR_PARAM_PROBE_LACK_DATA);
	}

	SHORT nModuType = SMT_UNKNOWN;	

	m_dFreqShift  = .0;
	m_dSymbolRate = .0;
	m_dSignalFreq = .0;
	//ZeroMemory(pSignalPara,sizeof(SIGNAL_DEMOD_PARAM));

	/// 信号参数测量 ///
	if (m_nModuType == SMT_UNKNOWN) 
	{
		// 调制样式未知 //
		nModuType = ModuRecognition(m_pDataBuff,m_dwSignLen);
	}
	else						
	{
		// 调制样式已知 //
		ParamEstimate(m_pDataBuff,m_dwSignLen);

		nModuType = m_nModuType;
	}	

	/// 判断软件配置中是否包含此种信号调制样式的分析功能 ///
	for (SHORT i = 0; i < 31; i++)
	{
		if (nModuType == g_pdllSysModuType[i])
		{
			pSignalPara.nModuType = nModuType;
			pSignalPara.dSignalFreq = m_dSignalFreq;
			pSignalPara.dSymbolRate = m_dSymbolRate;
			pSignalPara.dFreqShift  = m_dFreqShift;
			pSignalPara.dDuration = m_dwIQLen/m_dSamplingFreq/2;

			break;
		}
	}

	/// 重新初始化数据缓冲区 ///
	m_dwIQLen = 0;
	ZeroMemory(m_pDataBuff,m_dwSignLen*sizeof(double));

	return(ERROR_SUCCESS);
}

DWORD CSignalParamProbe::ParamEstimate( double *pData, DWORD dwDataLen )
{

	double pParam[6]={0.0};	

	switch(m_nModuType)
	{
	case SMT_SSB:
		{
			// 信号频率 //
			m_dSignalFreq = .0;
			// 信号带宽 //
			m_dSymbolRate = 2*0.85*m_dwFFTSize*m_dPasswidth/m_dSamplingFreq;
		}
		break;
	case SMT_CW:
	case SMT_AM:
	case SMT_FM:
		{
			// 信号幅度归一化 //
			if (!SignalNormalize(pData,dwDataLen))
			{
				return(ERROR_BUFFER_ALL_ZEROS);
			}
			// Signal Power Spectrum //
			CalAverPowerSpecrum(m_pSignPsd,pData,dwDataLen);
			GetSpectraParam(pParam,m_pSignPsd,m_dwFFTSize);
			m_dSignalFreq = pParam[5];
			m_dSymbolRate = 2*0.85*m_dwFFTSize*m_dPasswidth/m_dSamplingFreq;//pParam[4];			
		}
		break;
	case SMT_2ASK:
	case SMT_4ASK:
		{
			// 信号频率 //
			m_dSignalFreq = CalASKFrequncy(pData,dwDataLen);
			// 符号速率 //
			m_dSymbolRate = CalASKSymbolRate(pData,dwDataLen);
		}
		break;
	case SMT_2FSK:
	case SMT_4FSK:
		{
			CalFSKCFDF_InstFrequ(pParam,pData,dwDataLen);
			// 信号频率 //
			m_dSignalFreq = pParam[0];
			m_dFreqShift = pParam[1];
			// 符号速率 //
			m_dSymbolRate = CalFSKSymbolRate(pData,dwDataLen);
		}
		break;
	default:
		{
            memmove(m_pSignalBuff,pData,dwDataLen*sizeof(double));

			if (ERROR_SUCCESS == GetSpecAnalParam(m_pSignalBuff,dwDataLen))
			{
				double dTempValue = .0;
				///////////////////////////////////
				// 计算信号频率 //
				switch(m_nModuType)
				{
				case SMT_MSK:	
				case SMT_SDPSK:
				case SMT_PI4DQPSK:			
					{
						dTempValue = m_sap.nPeakIndex1 + m_sap.nPeakIndex2 - 1.0*m_dwFFTSize;
						m_dSignalFreq = dTempValue/2/pow(2.0,m_sap.nSpecType+1.0);
					}
					break;		
				default:
					{
						dTempValue = m_sap.nPeakIndex1 - 0.5*m_dwFFTSize;
						m_dSignalFreq = dTempValue/pow(2.0,m_sap.nSpecType+1.0);
					}
					break;
				}

				// 计算信号符号速率 //
				switch(m_nModuType)
				{
				case SMT_MSK:
				case SMT_GMSK:
					{
						// MSK平方谱 2 个峰值的距离为符号速率的1倍
						m_dSymbolRate = 2*abs(m_sap.nSquaPeakIdx2 - m_sap.nSquaPeakIdx1);
					}
					break;
				case SMT_OQPSK:
					{
						// OQPSK平方谱 2 个峰值的距离为符号速率的2倍
						m_dSymbolRate = abs(m_sap.nSquaPeakIdx2 - m_sap.nSquaPeakIdx1);
					}
					break;
				default:
					{
						// BPSK/QPSK/8PSK/16QAM/PI4DQPSK/SDPSK差分幅度谱的峰值位置
						m_dSymbolRate = m_sap.nDASPeakIdx;
					}
					break;
				}	
			}
		}
		break;
	}

	/// 信号频率 ///
	m_dSignalFreq *= m_dSamplingFreq/m_dwFFTSize;
	// 符号速率或信号带宽 ///
	m_dSymbolRate *= 0.5*m_dSamplingFreq/m_dwFFTSize;
	/// FSK频偏 ///
	m_dFreqShift *= m_dSamplingFreq/m_dwFFTSize;

	return(ERROR_SUCCESS);
}

SHORT CSignalParamProbe::ModuRecognition(double *pData, DWORD dwDataLen)
{
	SHORT nModuType;
	double dTempValue;
	double dBandwidth = 0.0, dAmplAveSqu,pParam[2]={0.0,0.0};	

	DWORD dwSucceed = ERROR_SUCCESS;

	nModuType = SMT_UNKNOWN;

    memmove(m_pSignalBuff,pData,dwDataLen*sizeof(double));

	//////////////////////////////////////////////////////////////////////////
	// 获取谱特征参数识别 MSK、BPSK、QPSK、8PSK、OQPSK、PI/4DQPSK、16QAM
 	dwSucceed = GetSpecAnalParam(m_pSignalBuff,dwDataLen);	

	dBandwidth = 1.0*m_sap.nBandwidth/m_dwFFTSize;

	dAmplAveSqu = 0.8413-1.6945*m_sap.dAmplAveSqu;

	if(dBandwidth>0.005 && dBandwidth<0.125 && m_sap.dAmplAveSqu<0.5)
	{		
		// BPSK的平方谱有1个峰值
		if ((m_sap.nSpecType == 0) && (m_sap.nSpecPeakNum == 1) && m_sap.dFMForOrder > 2.5)	
		{
			nModuType = SMT_BPSK;
		}

		// MSK的平方谱有2个峰值
		if( (m_sap.nSpecType == 0) && (m_sap.nSpecPeakNum == 2))//&& m_sap.dFMForOrder < 2.5)  
		{
			nModuType = SMT_MSK;	
		}	

		// PI/4DQPSK的四次方谱有2个峰值
		if ((m_sap.nSpecType == 1) && (m_sap.nSpecPeakNum == 2) && m_sap.dFMForOrder > 2.5)	
		{
			nModuType = SMT_PI4DQPSK;
		}

		// 8PSK的八次方谱峰值电平相对值大于10
		if ((m_sap.nSpecType == 2) && (m_sap.dSpecPeakLvl > 10) && m_sap.dFMForOrder > 2.5)
		{
			nModuType = SMT_8PSK;
		}

		// OQPSK的四次方谱有1个峰值，差分幅度谱峰值的相对电平小于0.05
		if ((m_sap.nSpecType == 1) && (m_sap.nSpecPeakNum == 1) && (m_sap.dDASPeakLvl < 0.05) && m_sap.dAMForOrder < 3.5)
		{
			nModuType = SMT_OQPSK;
		}

		// QPSK的四次方谱有 1 个峰值,差分幅度谱峰值的相对电平大于0.05
		if ((m_sap.nSpecType == 1) && (m_sap.nSpecPeakNum == 1) && ( m_sap.dDASPeakLvl > 0.05) && 
			(m_sap.dAmplAveSqu < 0.35 || m_sap.dDASPeakLvl < dAmplAveSqu) && m_sap.dFMForOrder > 2.5) 
		{
			nModuType = SMT_QPSK;
		}	

		// SDPSK的平方谱有2个峰值,差分幅度谱峰值的相对电平大于0.05
		if( (m_sap.nSpecType == 0) && (m_sap.nSpecPeakNum == 2) && (m_sap.dDASPeakLvl > 0.05) &&
			(m_sap.dFMForOrder > 2.0) && (m_sap.dAMForOrder > 2.5))  
		{
			nModuType = SMT_SDPSK;	
		}	

		// GMSK四次方谱有 1 个峰值,BT = 0.25
		if ((m_sap.nSpecType == 1) && (m_sap.nSpecPeakNum == 1) &&  (m_sap.dAmplAveSqu < 0.08) && (m_sap.dFMForOrder < 2.0) && 
			(m_sap.dFMForOrder > 1.5) && (m_sap.dAMForOrder > 2.45)) //&& m_sap.dFMStatWight > 0.5
		{
			nModuType = SMT_GMSK;
		}	

		// QAM的四次方谱有 1 个峰值,差分幅度谱峰值的相对电平大于0.05
		if ((m_sap.nSpecType == 1) && (m_sap.nSpecPeakNum == 1) && ( m_sap.dDASPeakLvl > 0.05) && m_sap.dAmplAveSqu > 0.35 )// && (m_sap.dDASPeakLvl > dAmplAveSqu)
		{
			nModuType = SMT_16QAM;
		}

		//---------------------------------------------------------------------
		// 16QAM的四次方谱有 1 个峰值,差分幅度谱峰值的相对电平大于0.05
		//if ((m_sap.nSpecType == 1) && (m_sap.nSpecPeakNum == 1) && ( m_sap.dDASPeakLvl > 0.05) && 
		//	m_sap.dAmplAveSqu > 0.35 && (m_sap.dDASPeakLvl > dAmplAveSqu) )
		//{
		//	nModuType = SMT_16QAM;
		//}

		//// 32QAM 的四次方谱有 1 个峰值,差分幅度谱峰值的相对电平大于0.05
		//if ( (m_sap.nSpecType == 1) && (m_sap.nSpecPeakNum == 1) && ( m_sap.dDASPeakLvl > 0.05) && m_sap.dAmplAveSqu > 0.35 && 				
		//	(m_sap.dFMForOrder > 2.5) && (m_sap.dAMForOrder < 2.5) && m_sap.dAmplAveSqu < 0.41)
		//{
		//	nModuType = SMT_32QAM;
		//}	

		//// 64QAM的四次方谱有 1 个峰值,差分幅度谱峰值的相对电平大于0.05
		//if ((m_sap.nSpecType == 1) && (m_sap.nSpecPeakNum == 1) && ( m_sap.dDASPeakLvl > 0.05) && m_sap.dAmplAveSqu > 0.35 && 	
		//	(m_sap.dFMForOrder > 2.5) && (m_sap.dAMForOrder < 2.5) && ( m_sap.dDASPeakLvl < 0.15))
		//{
		//	nModuType = SMT_64QAM;
		//}							

		//-----------------------------------------------------
		// 计算信号频率 //
		switch(nModuType)
		{
		case SMT_MSK:	
		case SMT_SDPSK:
		case SMT_PI4DQPSK:			
			{
				dTempValue = m_sap.nPeakIndex1 + m_sap.nPeakIndex2 - 1.0*m_dwFFTSize;
				m_dSignalFreq = dTempValue/2/pow(2.0,m_sap.nSpecType+1.0);
			}
			break;		
		default:
			{
				dTempValue = m_sap.nPeakIndex1 - 0.5*m_dwFFTSize;
				m_dSignalFreq = dTempValue/pow(2.0,m_sap.nSpecType+1.0);
			}
			break;
		}

		// 计算信号符号速率 //
		switch(nModuType)
		{
		case SMT_MSK:
		case SMT_GMSK:
			{
				// MSK平方谱 2 个峰值的距离为符号速率的1倍
				m_dSymbolRate = 2*abs(m_sap.nSquaPeakIdx2 - m_sap.nSquaPeakIdx1);
			}
			break;
		case SMT_OQPSK:
			{
				// OQPSK平方谱 2 个峰值的距离为符号速率的2倍
				m_dSymbolRate = abs(m_sap.nSquaPeakIdx2 - m_sap.nSquaPeakIdx1);
			}
			break;
		default:
			{
				// BPSK/QPSK/8PSK/16QAM/PI4DQPSK/SDPSK差分幅度谱的峰值位置
				m_dSymbolRate = m_sap.nDASPeakIdx;
			}
			break;
		}	

		//--------------------------------------
		// 高阶QAM信号识别
		//if (nModuType == SMT_16QAM)
		//{
		//	SHORT nPeakNum = QAMRecognition(pData,dwDataLen,2*m_dwFFTSize/m_dSymbolRate);
		//}
		//--------------------------------------------
		/// 频偏为0.5倍的符号速率的2FSK ///
		if (nModuType == SMT_MSK)
		{
			double dSymbolRate = CalFSKSymbolRate(pData,dwDataLen);

			if (fabs(dSymbolRate - m_dSymbolRate) > 0.25*dSymbolRate)
			{
				nModuType = SMT_2FSK;
				m_dFreqShift = m_dSymbolRate/4;
				m_dSymbolRate = dSymbolRate;
			}
		}
	}	

	//////////////////////////////////////////////////////////////
	// ASK、FSK、AM、FM、SSB、CW信号判决 //
	if (nModuType == SMT_UNKNOWN)
	{
		dBandwidth = 1.0*m_sap.dPsdLvlBnd/m_dwFFTSize;		

		// 噪声 //
		if ( m_sap.dPsdPeakDeT < 20 && m_sap.dAMForOrder > 2.5 && m_sap.dFMForOrder > 2.5 &&
			 m_sap.dAmplAveSqu > 0.5 && m_sap.dFreqAveSqu > 1.0 ) 
		{
			nModuType = SMT_NOISE;
		}	

		/// 信号源CW/SSB/AM/FM信号 //
		if ((dBandwidth < 0.001) && (m_sap.nPsdPeakNum == 1) && (m_sap.dPsdPeakDeT > 30)) 
		{
			nModuType = SMT_CW;
		}		

		// 单音FM调制
		if( (m_sap.dDFSPeakLvl - m_sap.dDASPeakLvl) > 0.2 && (m_sap.dFMForOrder < 2.5) && 
			(m_sap.dAMForOrder > 1.7) && (m_sap.dDASPeakLvl < 0.54)) 
		{
			nModuType = SMT_FM;
		}
		// 单音AM调制
		if( (m_sap.dDASPeakLvl - m_sap.dDFSPeakLvl) > 0.2 && m_sap.dFMForOrder > 1.5 && 
			m_sap.dAMForOrder < 2.5  && m_sap.dDASPeakLvl > 0.48)
		{
			nModuType = SMT_AM;
		} 		

		/// 音频/电台FM调制 ///
		if( m_sap.dFMForOrder < 3.5 && m_sap.dAMForOrder > 2.5  && m_sap.dAmplAveSqu < 0.15 && 
			m_sap.dFreqAveSqu > 0.28 && m_sap.dDASPeakLvl < 0.54 && m_sap.dPsdPeakDeT < 30) 
		{
			nModuType = SMT_FM;
		}

		/// 音频/电台AM调制 ///
		if( m_sap.dFMForOrder > 2.5 && m_sap.dAMForOrder < 5.0 && m_sap.dAmplAveSqu > 0.05 && 
			m_sap.dFreqAveSqu < 0.28 && m_sap.dDFSPeakLvl < 0.15)
		{
			nModuType = SMT_AM;
		} 	

		// SSB 带载波 //
		if( (nModuType == SMT_AM) && fabs(m_sap.dPsdSymmet) > 0.9 )
		{
			// m_sap.dPsdSymmet > 0  LSB
			// m_sap.dPsdSymmet < 0  USB
			nModuType = SMT_SSB;    	
		}

		// 电台SSB //
		//if ( m_sap.dPsdPeakDeT < 20 && m_sap.dAMStatWight > 0.5 &&
		//	m_sap.dFMForOrder > 2.5 && m_sap.dFMStatWight < 0.05 &&
		//	m_sap.dAmplAveSqu > 0.5 && m_sap.dFreqAveSqu < 0.85 &&
		//	m_sap.dDFSPeakLvl < 0.05) 
		//{
		//	nModuType = SMT_SSB;
		//}	

		///////////////////////////////////////////////////////////////////
		// 0.5 =< FD/SR <= 4 //
		if( (m_sap.dDFSPeakLvl - m_sap.dDASPeakLvl) < 0.2 && (m_sap.dFMForOrder < 1.7) && (m_sap.dAMForOrder > 2.5) && 
			(m_sap.dFreqAveSqu > 0.2) && (m_sap.dAmplAveSqu < 0.25) ) // && m_sap.dFMStatWight > 0.6
		{
			nModuType = SMT_2FSK;
		}		

		if(	(m_sap.dDFSPeakLvl - m_sap.dDASPeakLvl) < 0.2 && (m_sap.dFMForOrder > 1.7) && (m_sap.dFMForOrder < 2.5) &&
			(m_sap.dAMForOrder > 2.5) && (m_sap.dFreqAveSqu > 0.5) && (m_sap.dAmplAveSqu < 0.2))// && m_sap.dFMStatWight > 0.35 && m_sap.dFMStatWight < 0.65 
		{
			nModuType = SMT_4FSK;
		}

		// GMSK BT = 0.25
		if ((m_sap.nSpecType == 1) && (m_sap.nSpecPeakNum == 1) && (m_sap.dAmplAveSqu < 0.08) && (m_sap.dFMForOrder < 2.0) && 
			(m_sap.dFMForOrder > 1.5) && (m_sap.dAMForOrder > 2.45))// && m_sap.dFMStatWight > 0.5
		{
			nModuType = SMT_GMSK;
		}	

		if( m_sap.nPsdPeakNum == 1 && m_sap.dAMStatWight > 0.5 && m_sap.dAMForOrder < 1.5 && m_sap.dFMForOrder > 3.5 && 
			m_sap.dDFSPeakLvl < 0.05) 
		{
			nModuType = SMT_2ASK;
		}	

		// MPT-1327 二次调制为FM，FSK的高频能量大
		if( (m_sap.dFMForOrder < 1.7) && (m_sap.dAMForOrder < 2.0) && (m_sap.dFreqAveSqu > 0.25) && (m_sap.dAmplAveSqu < 0.25) &&
			m_sap.dFMStatWight > 0.55 && m_sap.dAMStatWight > 0.35)  
		{
			nModuType = SMT_2FSK;
		}	
		//////////////////////////////////////////////////////////////////

		switch(nModuType)
		{
		case SMT_CW:
		case SMT_SSB:
			{
				// 信号频率 //
				m_dSignalFreq = m_sap.nPsdPeakIdx1 - 0.5*m_dwFFTSize;
				// 信号带宽 //
				m_dSymbolRate = 2*dBandwidth*m_dwFFTSize;
			}
			break;
		case SMT_2FSK:		
			{
				if (m_sap.nPsdPeakNum == 2)
				{
					dTempValue = m_sap.nPsdPeakIdx1 + m_sap.nPsdPeakIdx2;
					m_dSignalFreq = 0.5*dTempValue - 0.5*m_dwFFTSize;
					m_dFreqShift = abs(m_sap.nPsdPeakIdx1 - m_sap.nPsdPeakIdx2)/2;
				}
				else
				{
					CalFSKCFDF_InstFrequ(pParam,pData,dwDataLen);
					m_dSignalFreq = pParam[0];
					m_dFreqShift  = pParam[1];
				}
				// 符号速率 //
				m_dSymbolRate = CalFSKSymbolRate(pData,dwDataLen);
			}
			break;
		case SMT_4FSK:
			{
				CalFSKCFDF_InstFrequ(pParam,pData,dwDataLen);
				m_dSignalFreq = pParam[0];
				m_dFreqShift  = pParam[1];			
				// 符号速率 //
				m_dSymbolRate = CalFSKSymbolRate(pData,dwDataLen);
			}
			break;
		case SMT_GMSK:
			{
				dTempValue = m_sap.nPeakIndex1 - 0.5*m_dwFFTSize;
				m_dSignalFreq = dTempValue/pow(2.0,m_sap.nSpecType+1.0);
				// MSK平方谱 2 个峰值的距离为符号速率的1倍
				m_dSymbolRate = 2*abs(m_sap.nSquaPeakIdx2 - m_sap.nSquaPeakIdx1);
			}
			break;
		case SMT_2ASK:
			{			
				if (m_sap.nPsdPeakNum == 1) 
				{
					m_dSignalFreq = m_sap.nPeakIndex1 - 0.5*m_dwFFTSize;
					m_dSignalFreq /= pow(2.0,m_sap.nSpecType+1.0);
				}
				else
				{
					m_dSignalFreq = m_sap.nPsdPeakIdx1 - 0.5*m_dwFFTSize;
				}
				// 符号速率 //
				m_dSymbolRate = CalASKSymbolRate(pData,dwDataLen);
			}
			break;
		default: //AM 、FM 
			{
				// 信号频率 //
				m_dSignalFreq = m_sap.dPsdCentFre;
				// 信号带宽 //
				m_dSymbolRate = 2*dBandwidth*m_dwFFTSize;
			}
		}			
	}
	//---------------------------------------------------
	// 信号频率 //
	m_dSignalFreq *= m_dSamplingFreq/m_dwFFTSize;
	// 符号速率或信号带宽 //
	m_dSymbolRate *= 0.5*m_dSamplingFreq/m_dwFFTSize;
	//  调制频偏 //
	m_dFreqShift *= m_dSamplingFreq/m_dwFFTSize;

	return(nModuType);    
}

// Spectrum Analysis Parameters //
DWORD CSignalParamProbe::GetSpecAnalParam(double *pData, DWORD dwDataLen)
{
	DWORD i,M;

	double dCenterFreq,dAmplAveSqu,dFreqAveSqu,dAMForthPar,dFMForthPar;
	LONG  nSignBandwid,nBandwidth;	

	double pReferParam[3] = {0,0,0};
	double pIdenPara[7]   = {0,0,0,0,0,0,0};
	double pSpecParam2[10];
	double pSpecParam4[10];
	double pSpecParam8[10];

	// Variable Initialization //
	dCenterFreq = 0.0;
	dAmplAveSqu = 0.0;
	dFreqAveSqu = 0.0;
	dAMForthPar = 0.0;
	dFMForthPar = 0.0;

	nSignBandwid = 0;
	nBandwidth   = 0;

	for(i=0;i<10;i++)
	{
		pSpecParam2[i] = 0;
		pSpecParam4[i] = 0;
		pSpecParam8[i] = 0;
	}		

	M = dwDataLen/2/64;

	// 信号幅度归一化 //
	if (!SignalNormalize(pData,dwDataLen))
	{
		return(ERROR_BUFFER_ALL_ZEROS);
	}

	// Signal Power Spectrum //
	CalAverPowerSpecrum(m_pSignPsd,pData,dwDataLen);	
	// Estimate center frequency and bandwidth //	
	RoughEstiFcBnw(dCenterFreq,nSignBandwid,m_pSignPsd);	

	m_sap.dCenterFreq	= dCenterFreq; 
	m_sap.nBandwidth	= nSignBandwid;

	// Get parameter threshold	 //
	nBandwidth = (DWORD)(nSignBandwid*0.5+0.5);
	if (nBandwidth < 5)
	{
		nBandwidth = 5;
	}			
	GetReferParam(pReferParam,m_pSignPsd,nBandwidth);	

	// 提取功率谱特征 //
	m_sap.dPsdSymmet   = GetSpectraSymmetParam(m_pSignPsd,m_dwFFTSize);;	
	GetSpectraParam(pIdenPara,m_pSignPsd,m_dwFFTSize);

	m_sap.nPsdPeakNum  = (LONG)pIdenPara[0];
	m_sap.nPsdPeakIdx1 = (LONG)pIdenPara[1];	
	m_sap.nPsdPeakIdx2 = (LONG)pIdenPara[2];	
	m_sap.dPsdPeakDeT  = pIdenPara[3];
	m_sap.dPsdLvlBnd   = pIdenPara[4];	
	m_sap.dPsdCentFre  = pIdenPara[5];	

	// 提取瞬时幅度特征//
	CalSingalEnvelope(pData,dwDataLen,m_pAmplBuff);
	dAmplAveSqu = GetAmplAveSqu(m_pAmplBuff,dwDataLen/2);		
	GetDiffSpecParam(pIdenPara,m_pAmplBuff,dwDataLen/2,nBandwidth);

	m_sap.dAmplAveSqu  = dAmplAveSqu;
	m_sap.dDASPeakLvl  = pIdenPara[0];
	m_sap.nDASPeakIdx  = (DWORD)pIdenPara[1];		

	// 提取瞬时频率特征//
	CalSingalFrequency(pData,dwDataLen,m_pFreqBuff);
	NormalizationAmplFreq(m_pAmplBuff,m_pFreqBuff,dwDataLen/2,M);
	dFreqAveSqu = GetFreqAveSqu(m_pFreqBuff,dwDataLen/2-M);		
	GetDiffSpecParam(pIdenPara,m_pFreqBuff,dwDataLen/2,nBandwidth,true);

	m_sap.dFreqAveSqu  = dFreqAveSqu;
	m_sap.dDFSPeakLvl  = pIdenPara[0];
	m_sap.nDFSPeakIdx  = (DWORD)pIdenPara[1];	

	//	Amplitudenormalization(m_pAmplBuff,dwDataLen/2); 
	dAMForthPar = GetFourthOrderParam(m_pAmplBuff,dwDataLen/2-M);
	//	Frequencynormalization(m_pFreqBuff,dwDataLen/2,M);
	dFMForthPar = GetFourthOrderParam(m_pFreqBuff,dwDataLen/2-M);

	m_sap.dAMForOrder = dAMForthPar;
	m_sap.dFMForOrder = dFMForthPar;

	m_sap.dAMStatWight = GetFrequencyStatParam(m_pAmplBuff,dwDataLen/2-M);
	m_sap.dFMStatWight = GetFrequencyStatParam(m_pFreqBuff,dwDataLen/2-M);

	// 信号平方谱分析 //
	CalSignalSquare(pData,dwDataLen);
	CalAverPowerSpecrum(m_pSignPsd,pData,dwDataLen);
	GetSpectrumParam(pSpecParam2,m_pSignPsd,nBandwidth,pReferParam,1);

	// 信号四次方谱分析 //
	CalSignalSquare(pData,dwDataLen);
	CalAverPowerSpecrum(m_pSignPsd,pData,dwDataLen);
	GetSpectrumParam(pSpecParam4,m_pSignPsd,nBandwidth,pReferParam,2);

	// 信号八次方谱分析 //
	CalSignalSquare(pData,dwDataLen);
	CalAverPowerSpecrum(m_pSignPsd,pData,dwDataLen);
	GetSpectrumParam(pSpecParam8,m_pSignPsd,nBandwidth,pReferParam,3);

	// 参数融合 //
	GetSpecIdenParam(pIdenPara,pSpecParam2,pSpecParam4,pSpecParam8);	

	m_sap.nSpecType    = (DWORD)pIdenPara[0];
	m_sap.nSpecPeakNum = (DWORD)pIdenPara[1];
	m_sap.dSpecPeakLvl = pIdenPara[2];
	m_sap.nPeakIndex1  = (DWORD)pIdenPara[3];
	m_sap.nPeakIndex2  = (DWORD)pIdenPara[4];
	m_sap.nSquaPeakIdx1 = (DWORD)pIdenPara[5];	
	m_sap.nSquaPeakIdx2 = (DWORD)pIdenPara[6];	

	return(ERROR_SUCCESS);	
}


// Calculate Average Power Spectrum of signal
DWORD CSignalParamProbe::CalAverPowerSpecrum(double* pPoweSpec,double *pData, DWORD dwDataLen)
{
    DWORD i = 0;
	double dNormFactor;

	ZeroMemory(pPoweSpec,m_dwFFTSize*sizeof(double));

	DWORD dwSpecLen = 0;
	m_pFFTIQ->InputData(pData,dwDataLen);
	double*pSpecBuf = m_pFFTIQ->GetSpectrum(dwSpecLen);

	if (pSpecBuf != NULL)
	{
        memmove(pPoweSpec,pSpecBuf,m_dwFFTSize*sizeof(double));

		//Spectrum Normalization
		dNormFactor = 0.0;
		for(i=0;i<m_dwFFTSize;i++) 
		{
			dNormFactor += pPoweSpec[i]*pPoweSpec[i];	
		}
		for(i=0;i<m_dwFFTSize;i++)
		{
			pPoweSpec[i] = pPoweSpec[i]*pPoweSpec[i]/dNormFactor;	
		}
	}	

	return(ERROR_SUCCESS);
}

// Calculate Amplitude Average Power Spectrum of signal
void CSignalParamProbe::CalAmplAverPowerSpecrum(double* pPoweSpec,double *pData, DWORD dwDataLen)
{
	DWORD i;
	double dNormFactor;

	ZeroMemory(pPoweSpec,m_dwFFTSize*sizeof(double));

	DWORD dwSpecLen = 0;
	m_pFFTAD->InputData(pData,dwDataLen);
	double*pSpecBuf = m_pFFTAD->GetSpectrum(dwSpecLen);

	if (pSpecBuf != NULL)
	{
        memmove(pPoweSpec,pSpecBuf,dwSpecLen*sizeof(double));

		//Spectrum Normalization
		dNormFactor = 0.0;
		for(i=0;i<dwSpecLen;i++) 
		{
			dNormFactor += pPoweSpec[i]*pPoweSpec[i];	
		}
		for(i=0;i<dwSpecLen;i++)
		{
			pPoweSpec[i] = pPoweSpec[i]*pPoweSpec[i]/dNormFactor;	
		}
	}	
}

// Rough calculation for signal center frequency and bandwidth
void CSignalParamProbe::RoughEstiFcBnw(double& dCenterFreq,LONG& nBandwidth,double *pSignalPsd)
{
	DWORD i;
	double dPsdMax,dPsdSum;
	DWORD nMaxIndex;
	DWORD nStart,nEnd,nLen,nMinLen;

	double dCenterSum;
	double dRightSum,dLeftSum,dTemp;

	double dScaleThreshold= 25; //min.samples of signal bandwidth
	double dBnwThreshold  = 0.5;//threshold of signal bandwidth energy 
	double dPowerThreshold= 8;	//Threshold of power 3dB	

	////calculate the maximum of Signal Power Spectrum////
	dPsdMax   = .0;
	nMaxIndex = GetMaxIndex(pSignalPsd,m_dwFFTSize,dPsdMax);

	//calculate the sum energy of Signal Power Spectrum
	dPsdSum = 0.0;	
	for(i=0;i<m_dwFFTSize;i++)
	{
		dPsdSum += pSignalPsd[i];
	}

	nStart = nMaxIndex-1; //Start position of signal bandwidth
	nEnd   = nMaxIndex+1; //End position of signal bandwidth
	nLen = m_dwFFTSize-nMaxIndex-(DWORD)dScaleThreshold;
	if (nLen > nMaxIndex-(DWORD)dScaleThreshold)
	{
		nLen = nMaxIndex-(DWORD)dScaleThreshold;
	}

	dCenterSum = dPsdMax;	
	nMinLen = (DWORD)dScaleThreshold; 	
	while(nLen>=0 && dCenterSum/dPsdSum<dBnwThreshold)
	{
		dRightSum=0.0;
		for(i=nEnd;i<nEnd+nMinLen;i++) 
		{
			dRightSum += pSignalPsd[i];
		}

		dLeftSum=0.0;
		for(i=nStart;i>nStart-nMinLen;i--)
		{
			dLeftSum += pSignalPsd[i];
		}

		if(dRightSum>dLeftSum)
		{
			dCenterSum += pSignalPsd[nEnd];
			nEnd++;
		}
		else if(dRightSum<dLeftSum)
		{
			dCenterSum += pSignalPsd[nStart];
			nStart--;
		}
		else
		{
			dCenterSum += pSignalPsd[nStart]+pSignalPsd[nEnd];
			nEnd++;
			nStart--;
		}

		nLen--;

		dRightSum=0.0;
		for(i=nEnd;i<nEnd+nMinLen;i++) 
		{
			dRightSum += pSignalPsd[i];
		}
		dRightSum /= nMinLen;

		dLeftSum=0.0;
		for(i=nStart;i>nStart-nMinLen;i--)
		{
			dLeftSum += pSignalPsd[i];
		}
		dLeftSum /= nMinLen;

		dTemp=0.0;
		for(i=nStart+1;i<nEnd-1;i++)
		{
			dTemp += pSignalPsd[i];
		}
		dTemp /= nEnd-nStart-1;

		if(dTemp/dRightSum>dPowerThreshold && dTemp/dLeftSum>dPowerThreshold)
		{
			break;
		}
	}

	// Rough calculation for signal center frequency
	dCenterFreq = (nEnd+nStart+2-1.0*m_dwFFTSize)/2;
	// Rough calculation for signal bandwidth		 
	nBandwidth = nEnd-nStart+1;
}

void CSignalParamProbe::GetReferParam(double *pReferParam,double *pSignalPsd,LONG nBandwidth)
{
	DWORD i,ki,nCounter,nFindLen;

	double dTempValue;
	double dMinValue,dMaxValue;
	double dParameter1,dParameter2,dParameter3;

	memset(m_pBuffer1,0,m_dwFFTSize*sizeof(double));	
	memset(m_pBuffer2,0,m_dwFFTSize*sizeof(double));
	memset(m_pBuffer3,0,m_dwFFTSize*sizeof(double));

	nFindLen = 0;
	dTempValue = 0.0;
	for(i = nBandwidth;i<m_dwFFTSize-nBandwidth;i++)
	{
		if((pSignalPsd[i] > pSignalPsd[i+1]) && (pSignalPsd[i] > pSignalPsd[i-1]))
		{
			if(pSignalPsd[i-1] > pSignalPsd[i+1])
			{
				m_pBuffer1[nFindLen] = pSignalPsd[i]+pSignalPsd[i-1];
			}
			else
			{
				m_pBuffer1[nFindLen] = pSignalPsd[i]+pSignalPsd[i+1];
			}

			dTempValue=0.0;
			for(ki=i-nBandwidth;ki<i+nBandwidth+1;ki++)
			{
				dTempValue += pSignalPsd[ki];
			}

			m_pBuffer2[nFindLen] = (dTempValue-m_pBuffer1[nFindLen])/(2*nBandwidth-1);

			nFindLen++;
		}
	}

	for(i=0;i<nFindLen;i++) 
	{
		m_pBuffer3[i] = m_pBuffer1[i]/m_pBuffer2[i];

		if(m_pBuffer1[i]-m_pBuffer2[i]<0)
		{
			m_pBuffer2[i]=0;
		}
		else
		{
			m_pBuffer2[i] = m_pBuffer1[i]-m_pBuffer2[i];
		}
	}

	dMaxValue = GetMaxVal(m_pBuffer2,nFindLen);
	dMinValue = GetMinVal(m_pBuffer2,nFindLen);
	dTempValue = dMinValue+0.75*(dMaxValue-dMinValue);

	dParameter1=0.0;
	dParameter2=0.0;
	dParameter3=0.0;

	nCounter=0;
	for(i=0;i<nFindLen;i++)
	{
		if(m_pBuffer2[i]>dTempValue)
		{
			dParameter1 += m_pBuffer1[i];
			dParameter2 += m_pBuffer2[i];
			dParameter3 += m_pBuffer3[i];
			nCounter++;
		}
	}
	dParameter1 /= nCounter;
	dParameter2 /= nCounter;
	dParameter3 /= nCounter;


	pReferParam[0] = dParameter1;
	pReferParam[1] = dParameter2;
	pReferParam[2] = dParameter3;
}

void CSignalParamProbe::GetSpectrumParam(double *pSpecParam,double *pSignalPsd,LONG nBandwidth,double *pReferParam,
										 short N)
{
	DWORD i,k,ki;
	DWORD nCounter1,nCounter2,nCounter3,nNum;
	DWORD nFindLen,nTempIndex;
	double dLimit,dTempValue;
	double dMax1,dMax2,dMax3;

	memset(m_pBuffer1,0,m_dwFFTSize*sizeof(double));	
	memset(m_pBuffer2,0,m_dwFFTSize*sizeof(double));
	memset(m_pBuffer3,0,m_dwFFTSize*sizeof(double));
	memset(m_pPsdInex,0,m_dwFFTSize*sizeof(LONG));

	nFindLen=0;
	for(i=nBandwidth;i<m_dwFFTSize-nBandwidth;i++)
	{
		if(pSignalPsd[i]>pSignalPsd[i+1] && pSignalPsd[i]>pSignalPsd[i-1])
		{
			if(pSignalPsd[i-1]>pSignalPsd[i+1])
			{
				m_pBuffer1[nFindLen] = pSignalPsd[i]+pSignalPsd[i-1];
			}
			else
			{
				m_pBuffer1[nFindLen] = pSignalPsd[i]+pSignalPsd[i+1];				
			}

			m_pPsdInex[nFindLen] = i;

			nFindLen++;
		}
	}
	if (nFindLen < 1)
	{
		return;
	}

	dLimit = pReferParam[0]/pow(2.0,N);

	nCounter1 = 0;
	for(i=0;i<nFindLen;i++)
	{
		if(m_pBuffer1[i]>dLimit)
		{
			m_pBuffer1[nCounter1] = m_pBuffer1[i];

			k = m_pPsdInex[i];
			dTempValue = 0.0;
			for(ki=k-nBandwidth;ki<k+nBandwidth+1;ki++)
			{
				dTempValue += pSignalPsd[ki];
			}

			m_pPsdInex[nCounter1] = m_pPsdInex[i];

			m_pBuffer2[nCounter1]  = (dTempValue-m_pBuffer1[nCounter1])/(2*nBandwidth-1);

			m_pBuffer3[nCounter1]  = m_pBuffer1[nCounter1]-m_pBuffer2[nCounter1];

			nCounter1++;
		}
	}

	if (nCounter1 < 1)
	{
		return;
	}	


	dLimit = pReferParam[1]/pow(2.0,N);
	nCounter2 = 0;
	for(i=0;i<nCounter1;i++)
	{
		if(m_pBuffer3[i]>dLimit)
		{
			m_pPsdInex[nCounter2] = m_pPsdInex[i];
			m_pBuffer1[nCounter2] = m_pBuffer1[i];
			m_pBuffer3[nCounter2] = m_pBuffer3[i];

			m_pBuffer2[nCounter2] = m_pBuffer1[i]/m_pBuffer2[i];

			nCounter2++;
		}
	}


	if (nCounter2 < 1)
	{
		return;
	}	

	dLimit = pReferParam[2];
	nCounter3 = 0;	
	for(i=0;i<nCounter2;i++)
	{
		if(m_pBuffer2[i]>dLimit)
		{	
			m_pPsdInex[nCounter3] = m_pPsdInex[i];
			m_pBuffer1[nCounter3] = m_pBuffer1[i];
			m_pBuffer2[nCounter3] = m_pBuffer2[i];
			m_pBuffer3[nCounter3] = m_pBuffer3[i];


			nCounter3++;
		}
	}

	dTempValue = 0.0;
	nTempIndex = 0;
	for(i=0;i<nCounter3;i++)
	{
		dTempValue = m_pBuffer2[i];
		nTempIndex = i;
		for(k=i+1;k<nCounter3;k++)
		{
			if(m_pBuffer2[k] > dTempValue)
			{
				dTempValue = m_pBuffer2[k];
				nTempIndex = k;							
			}					
		}	

		if(i!=nTempIndex)
		{
			m_pBuffer2[nTempIndex] = m_pBuffer2[i];
			m_pBuffer2[i] = dTempValue;

			dTempValue = m_pBuffer1[i];
			m_pBuffer1[i] = m_pBuffer1[nTempIndex];
			m_pBuffer1[nTempIndex] = dTempValue;

			dTempValue = m_pBuffer3[i];
			m_pBuffer3[i] = m_pBuffer3[nTempIndex];
			m_pBuffer3[nTempIndex] = dTempValue;

			m_pPsdInex[i] += m_pPsdInex[nTempIndex];
			m_pPsdInex[nTempIndex] = m_pPsdInex[i]-m_pPsdInex[nTempIndex];
			m_pPsdInex[i] -= m_pPsdInex[nTempIndex];
		}
	}

	dMax1 = GetMaxVal(m_pBuffer1,nCounter3);
	dMax2 = GetMaxVal(m_pBuffer2,nCounter3);
	dMax3 = GetMaxVal(m_pBuffer3,nCounter3);

	nNum = 0;
	for(i=0;i<nCounter3;i++)
	{
		if(m_pBuffer1[i]>0.5*dMax1 && m_pBuffer2[i]>0.5*dMax2 && m_pBuffer3[i]>0.5*dMax3)
		{
			nNum++;
		}
	}

	// the number of power spectrum peaks
	pSpecParam[0] = nNum*1.0;

	// distance between  1st peak and 2nd peak
	if(nNum>1)	
	{
		pSpecParam[1] = abs(m_pPsdInex[0]-m_pPsdInex[1])*1.0;
	}
	else
	{
		pSpecParam[1]=0;
	}

	// parameters of 1st peak
	pSpecParam[2] = m_pBuffer2[0]; // absolute level/relative level
	pSpecParam[3] = m_pPsdInex[0]; // max.peak index	
	pSpecParam[4] = m_pBuffer1[0]; // max.peak absolute level (absolute value)
	pSpecParam[5] = m_pBuffer3[0]; // max.peak relative level (relative value)

	// parameters of 2nd peak
	if(nNum>1)
	{
		pSpecParam[6] = m_pBuffer2[1];
		pSpecParam[7] = m_pPsdInex[1];
		pSpecParam[8] = m_pBuffer1[1];
		pSpecParam[9] = m_pBuffer3[1];
	}

}

void CSignalParamProbe::GetSpecIdenParam(double* pIdenPara,double *pSpecParam2,double *pSpecParam4,double *pSpecParam8)
{
	int i;
	double dMaxValue;
	double dNormal1,dNormal2;

	double pPara1[30];
	double pPara2[6] = {0,0,0,0,0,0};

	int nIndex[4];
	int nFindIndex;

	for(i=0;i<10;i++)
	{
		pPara1[i]   =pSpecParam2[i];
		pPara1[10+i]=pSpecParam4[i];
		pPara1[20+i]=pSpecParam8[i];
	}

	for(i=0;i<3;i++)
	{
		pPara2[2*i]  =pPara1[10*i+4];
		pPara2[2*i+1]=pPara1[10*i+5];
	}

	dMaxValue=0.0;	
	for(i=0;i<6;i++)
	{
		if(dMaxValue < pPara2[i])	
		{
			dMaxValue = pPara2[i];
		}
	}

	if(dMaxValue>0.00000000001)
	{
		dNormal1 = 0.0f;
		dNormal2 = 0.0f;

		for(i=0;i<3;i++)
		{
			dNormal1 += pPara2[2*i]*pPara2[2*i];
			dNormal2 += pPara2[2*i+1]*pPara2[2*i+1];
		}

		for(i=0;i<3;i++)
		{
			pPara2[2*i] /= sqrt(dNormal1);
			pPara2[2*i+1] /= sqrt(dNormal2);
		}

		for(i=0;i<3;i++)
		{
			pPara2[2*i]=pPara2[2*i]*pPara2[2*i]+pPara2[2*i+1]*pPara2[2*i+1];
		}		

		dMaxValue=0.0;
		for(i=0;i<3;i++)
		{			
			if(dMaxValue<pPara1[10*i+2])
			{
				dMaxValue = pPara1[10*i+2];
				nIndex[0] = i;
			}
		}

		dMaxValue=0.0;
		for(i=0;i<3;i++)
		{			
			if(dMaxValue<pPara1[10*i+4])
			{
				dMaxValue = pPara1[10*i+4];
				nIndex[1] = i;
			}
		}

		dMaxValue=0.0;
		for(i=0;i<3;i++)
		{			
			if(dMaxValue<pPara1[10*i+5])
			{
				dMaxValue = pPara1[10*i+5];
				nIndex[2] = i;
			}
		}

		dMaxValue=0.0;
		for(i=0;i<3;i++)
		{			
			if(dMaxValue<pPara2[2*i])
			{
				dMaxValue = pPara2[2*i];
				nIndex[3] = i;
			}
		}

		if(nIndex[0]==nIndex[1] && nIndex[0]==nIndex[2]) 
		{
			nFindIndex = nIndex[0];
		}
		else if(nIndex[1]==nIndex[2]) 
		{
			nFindIndex = nIndex[1];
		}
		else
		{
			nFindIndex = nIndex[3];
		}		

		//////////////////////////////////////////////////////////////////////////
		pIdenPara[0] = nFindIndex;
		pIdenPara[1] = pPara1[10*nFindIndex]; 
		pIdenPara[2] = pPara1[10*nFindIndex+2]; 
		pIdenPara[3] = pPara1[10*nFindIndex+3];
		pIdenPara[4] = pPara1[10*nFindIndex+7];
		pIdenPara[5] = pSpecParam2[3];		
		pIdenPara[6] = pSpecParam2[7];
	}	
}

void CSignalParamProbe::GetDiffSpecParam(double* pIdenPara,double* pSignAmpl,DWORD dwDataLen,LONG nBandwidth,bool bIsFreq)
{
	LONG i,ki,nCounter;
	int nFindLen,nLen;
	int nMaxIndex,nNum;
	double dTempValue,dMaxValue;
	double dNormal2,dNormal3;
	double dMax1,dMax2,dMax3;

	//Amplitude difference
	m_pTmpBufI[0] = 0;
	if (bIsFreq)
	{
		for (DWORD n=1;n<dwDataLen;n++)
		{
			m_pTmpBufI[n] = fabs(pSignAmpl[n]) - fabs(pSignAmpl[n-1]);
		}
	}
	else
	{
		for (DWORD n=1;n<dwDataLen;n++)
		{
			m_pTmpBufI[n] = pSignAmpl[n] - pSignAmpl[n-1];
		}
	}	

	//Calculate difference amplitude power spectrum	
	CalAmplAverPowerSpecrum(m_pAmplPSD,m_pTmpBufI,dwDataLen);

	memset(m_pBuffer1,0,m_dwFFTSize*sizeof(double));	
	memset(m_pBuffer2,0,m_dwFFTSize*sizeof(double));
	memset(m_pBuffer3,0,m_dwFFTSize*sizeof(double));
	memset(m_pPsdInex,0,m_dwFFTSize*sizeof(LONG));

	dTempValue = 0.0;
	nFindLen = 0;
	nLen = m_dwFFTSize - nBandwidth;
	for(i = nBandwidth; i < nLen; i++)
	{
		if(m_pAmplPSD[i]>m_pAmplPSD[i+1] && m_pAmplPSD[i]>m_pAmplPSD[i-1])
		{
			if(m_pAmplPSD[i-1]>m_pAmplPSD[i+1])
			{
				m_pBuffer1[nFindLen] = m_pAmplPSD[i]+m_pAmplPSD[i-1];
			}
			else
			{
				m_pBuffer1[nFindLen] = m_pAmplPSD[i]+m_pAmplPSD[i+1];
			}

			dTempValue=0.0;
			for(ki = i - nBandwidth; ki < i + nBandwidth+1; ki++)
			{
				dTempValue += m_pAmplPSD[ki];
			}

			m_pBuffer2[nFindLen] = (dTempValue-m_pBuffer1[nFindLen])/(2*nBandwidth-1);

			m_pPsdInex[nFindLen]=i;

			nFindLen++;
		}
	}


	for(i=0;i<nFindLen;i++) 
	{
		m_pBuffer3[i] = m_pBuffer1[i]/m_pBuffer2[i];

		if(m_pBuffer1[i]<m_pBuffer2[i])
		{
			m_pBuffer2[i] = 0.0;
		}
		else
		{
			m_pBuffer2[i] = m_pBuffer1[i]-m_pBuffer2[i];
		}
	}

	dMaxValue = GetMaxVal(m_pBuffer3,nFindLen);

	nCounter=0;
	for(i=0;i<nFindLen;i++) 
	{
		if(m_pBuffer3[i]>0.5*dMaxValue)
		{
			m_pBuffer3[nCounter] = m_pBuffer3[i];
			m_pBuffer2[nCounter] = m_pBuffer2[i];
			m_pBuffer1[nCounter] = m_pBuffer1[i];
			m_pPsdInex[nCounter]= m_pPsdInex[i];
			nCounter++;
		}
	}

	dNormal2 = 0.0f;
	dNormal3 = 0.0f;
	for(i=0;i<nCounter;i++)
	{
		dNormal2 += m_pBuffer2[i]*m_pBuffer2[i];
		dNormal3 += m_pBuffer3[i]*m_pBuffer3[i];
	}
	dNormal2 = sqrt(dNormal2);
	dNormal3 = sqrt(dNormal3);

	for(i=0;i<nCounter;i++)
	{		
		m_pTmpBufI[i] = (m_pBuffer2[i]/dNormal2)*(m_pBuffer2[i]/dNormal2);
		m_pTmpBufI[nCounter+i] = (m_pBuffer3[i]/dNormal3)*(m_pBuffer3[i]/dNormal3);
	}

	nMaxIndex = 0;
	dMaxValue = 0.0;
	nMaxIndex = GetMaxIndex(m_pTmpBufI,nCounter,dMaxValue);	

	dMax1 = GetMaxVal(m_pBuffer1,nCounter);
	dMax2 = GetMaxVal(m_pBuffer2,nCounter);
	dMax3 = GetMaxVal(m_pBuffer3,nCounter);

	nNum = 0;
	for(i=0;i<nCounter;i++)
	{
		if(m_pBuffer1[i]>0.25*dMax1 && m_pBuffer2[i]>0.25*dMax2 && m_pBuffer3[i]>0.75*dMax3)
		{
			nNum++;
		}
	}

	pIdenPara[0] = m_pBuffer2[nMaxIndex];
	pIdenPara[1] = m_pPsdInex[nMaxIndex];
	pIdenPara[2] = m_pBuffer1[nMaxIndex];
	pIdenPara[3] = m_pBuffer3[nMaxIndex];
	pIdenPara[4] = nNum;
}


double CSignalParamProbe::GetAmplAveSqu(double* pSignAmpl,DWORD dwDataLen)	
{
	DWORD i,nFrom;
	double dMeanValue,dTemp;
	double dMeanSquare;
	double dSquareMean;

	nFrom = 0;//(dwDataLen/8)>256 ? 256:(dwDataLen/8);

	dMeanValue = 0.0;
	for (i=nFrom;i<dwDataLen;i++)
	{
		dMeanValue += pSignAmpl[i];
	}
	dMeanValue /= (dwDataLen-nFrom);

	dMeanSquare = 0.0;
	for (i=nFrom;i<dwDataLen;i++)
	{
		dMeanSquare += *(pSignAmpl+i)/dMeanValue - 1;
	}
	dMeanSquare /= (dwDataLen-nFrom);
	dMeanSquare = dMeanSquare*dMeanSquare;

	dSquareMean = 0.0;
	for (i=nFrom;i<dwDataLen;i++)
	{
		dTemp = *(pSignAmpl+i)/dMeanValue - 1;
		dSquareMean += dTemp*dTemp;
	}
	dSquareMean /= (dwDataLen-nFrom);

	dTemp = sqrt(dSquareMean-dMeanSquare);

	return(dTemp);
}


double CSignalParamProbe::GetFreqAveSqu( double* pSignFreq, DWORD dwDataLen )
{
	DWORD i;
	double dTmpVal;

	for (i=0;i<dwDataLen;i++)
	{
		m_pTmpBufI[i] = fabs(*(pSignFreq+i));
	}

	dTmpVal = GetAmplAveSqu(m_pTmpBufI,dwDataLen);

	return(dTmpVal);
}

void CSignalParamProbe::NormalizationAmplFreq(double* pAmplBuff,
											  double* pFreqBuff,
											  int N,int M )
{
	int i,k,Index1,Index2;
	double dMinLvl0,dMaxLvl0,dMaxLvl,dMinLvl,dTmpVal;	

	//////////////////////////////////////////////////////////////////////////
	/// 检测非弱信号M/2 //

	dMinLvl0 = pAmplBuff[0];
	dMaxLvl0 = pAmplBuff[0];
	for(i = 0 ;i < N;i++)
	{
		if (dMinLvl0 > pAmplBuff[i])
		{
			dMinLvl0 = pAmplBuff[i];
		}	

		if (dMaxLvl0 < pAmplBuff[i])
		{
			dMaxLvl0 = pAmplBuff[i];
		}	
	}

	for(k = 0; k < M/2; k++)
	{
		Index1 = 0;
		dMinLvl = pAmplBuff[0];

		for(i = 0 ;i < N - k;i++)
		{
			if (dMinLvl > pAmplBuff[i] && pAmplBuff[i] > FLT_EPSILON)
			{
				dMinLvl = pAmplBuff[i];
				Index1 = i;
			}	
		}

		if (dMinLvl > 0.25*(dMinLvl0+dMaxLvl0))
		{
			break;
		}
		pAmplBuff[Index1] = 0.0;
		pFreqBuff[Index1] = 0.0;
	}	

	Index1 = 0;
	for(i = 0 ;i<N;i++)
	{
		if (fabs(pAmplBuff[i]) > FLT_EPSILON)
		{
			pAmplBuff[Index1] = pAmplBuff[i];
			pFreqBuff[Index1] = pFreqBuff[i];
			Index1++;
		}
	}

	for(i = Index1; i<N; i++)
	{
		pAmplBuff[i] = 0.0;
		pFreqBuff[i] = 0.0;
	}

	//////////////////////////////////////////////////////////////////////
	/// 瞬时去野值 M/2 //
	for(k = 0; k < M/4; k++)
	{
		Index1 = 0;
		Index2 = 0;
		dMaxLvl = pFreqBuff[0];
		dMinLvl = pFreqBuff[0];

		for(i = 0 ;i<N;i++)
		{
			if (dMaxLvl < pFreqBuff[i])
			{
				dMaxLvl = pFreqBuff[i];
				Index1 = i;
			}	

			if (dMinLvl > pFreqBuff[i])
			{
				dMinLvl = pFreqBuff[i];
				Index2 = i;
			}	
		}

		pFreqBuff[Index1] = 0.0;
		pFreqBuff[Index2] = 0.0;
	}	

	Index1 = 0;
	for(i = 0 ;i<N;i++)
	{
		if (fabs(pFreqBuff[i]) > FLT_EPSILON)
		{
			pFreqBuff[Index1] = pFreqBuff[i];
			Index1++;
		}
	}

	for(i = Index1; i<N; i++)
	{
		pFreqBuff[i] = 0.0;
	}	

	// 瞬时频率归一化 除均方根//
	dTmpVal = 0.0;
	for (i = 0; i < N - M; i++)
	{
		dTmpVal += pFreqBuff[i]*pFreqBuff[i];		
	}
	dTmpVal = sqrt(dTmpVal/(N-M));
	dTmpVal  = 1/dTmpVal;	

	for (i=0; i < N - M; i++) 
	{
		pFreqBuff[i] *= dTmpVal; 
	}	
}

void CSignalParamProbe::Frequencynormalization( double* pFreqBuff,int N,int M )
{
	int i,k,Index1,Index2;
	double dMaxLvl,dMinLvl,dTmpVal;	

	/// 去野值 //
	for(k = 0; k<M/2; k++)
	{
		Index1 = 0;
		Index2 = 0;
		dMaxLvl = pFreqBuff[0];
		dMinLvl = pFreqBuff[0];

		for(i = 0 ;i<N;i++)
		{
			if (dMaxLvl < pFreqBuff[i])
			{
				dMaxLvl = pFreqBuff[i];
				Index1 = i;
			}	

			if (dMinLvl > pFreqBuff[i])
			{
				dMinLvl = pFreqBuff[i];
				Index2 = i;
			}	
		}

		pFreqBuff[Index1] = 0.0;
		pFreqBuff[Index2] = 0.0;
	}	

	Index1 = 0;
	for(i = 0 ;i<N;i++)
	{
		if (fabs(pFreqBuff[i]) > FLT_EPSILON)
		{
			pFreqBuff[Index1] = pFreqBuff[i];
			Index1++;
		}
	}

	for(i = Index1; i<N; i++)
	{
		pFreqBuff[i] = 0.0;
	}

	// 归一化 除均方根//
	dTmpVal = 0.0;
	for (i = 0; i < N - M; i++)
	{
		dTmpVal += pFreqBuff[i]*pFreqBuff[i];		
	}
	dTmpVal = sqrt(dTmpVal/(N-M));
	dTmpVal  = 1/dTmpVal;	

	for (i=0; i < N - M; i++) 
	{
		pFreqBuff[i] *= dTmpVal; 
	}	
}

void CSignalParamProbe::Amplitudenormalization(double* pAmplBuff,int N)
{
	int i;
	double dTmpVal; 	

	// 归一化 除均方根//
	dTmpVal = 0.0;
	for (i = 0; i < N; i++)
	{
		dTmpVal += pAmplBuff[i]*pAmplBuff[i];		
	}
	dTmpVal = sqrt(dTmpVal/N);
	dTmpVal  = 1/dTmpVal;	

	for (i=0; i < N; i++) 
	{
		pAmplBuff[i] *= dTmpVal; 
	}	
}


double CSignalParamProbe::GetFrequencyStatParam( double* pInput,int Inlen )
{
	double dFMWight = 0.0;

	LONG i,dwFrom,dwTo,K;
	/////////////////////////////////////////
	// 求非弱信号的瞬时幅度范围 //
	dwFrom = 0;
	dwTo   = Inlen;
	double dMaxLevel = pInput[dwFrom];
	double dMinLevel = pInput[dwFrom];
	for(i = dwFrom ;i<dwTo;i++)
	{
		if (dMaxLevel < pInput[i])
		{
			dMaxLevel = pInput[i];
		}
		if (dMinLevel > pInput[i])
		{
			dMinLevel = pInput[i];
		}
	}

	// 统计非弱信号的瞬时幅度分布 //
	SHORT N = 100;
	double dFactor = N/(dMaxLevel - dMinLevel);
	memset(m_pTmpBufI,0,m_dwSignLen*sizeof(double)/2);

	for(i = dwFrom ;i<dwTo;i++)
	{
		K = SHORT(dFactor*(pInput[i] - dMinLevel));
		m_pTmpBufI[K] += 1.0;
	}	

	// 求分布的最大值 //
	SHORT kn,nIdex1,nIdex2;
	double dMaxVal1,dMaxVal2,dSumVal0,dSumVal1,dSumVal2;

	dSumVal0 = m_pTmpBufI[0];
	for(kn = 1; kn < N; kn++)
	{
		m_pTmpBufI[kn] = (m_pTmpBufI[kn] + m_pTmpBufI[kn-1])/2;		
		dSumVal0 += m_pTmpBufI[kn];
	}	
	for(kn = 0; kn < N; kn++)
	{
		m_pTmpBufI[kn] /= dSumVal0;
	}

	//FILE* fp1;
	//fp1 = fopen("F:\\peak.bin","wb");
	//fwrite(m_pTmpBufI,sizeof(double),N,fp1);
	//fclose(fp1);

	nIdex1 = 3*N/4;
	dSumVal1 = 0;
	dMaxVal1 = m_pTmpBufI[nIdex1];
	for (kn = 3*N/4;kn < N;kn++)
	{
		dSumVal1 += m_pTmpBufI[kn];
		if (dMaxVal1 < m_pTmpBufI[kn])
		{
			dMaxVal1 = m_pTmpBufI[kn];
			nIdex1 = kn;
		}
	}

	nIdex2 = 0;
	dSumVal2 = 0;
	dMaxVal2 = m_pTmpBufI[0];
	for (kn = 0;kn < N/4;kn++)
	{
		dSumVal2 += m_pTmpBufI[kn];
		if (dMaxVal2 < m_pTmpBufI[kn])
		{
			dMaxVal2 = m_pTmpBufI[kn];
			nIdex2 = kn;
		}
	}

	dFMWight = dSumVal1 + dSumVal2;

	return dFMWight;
}


//零中心归一化瞬时包络/频率的紧致性(四阶矩) //
double CSignalParamProbe::GetFourthOrderParam(double *pInput,int len)
{	
	int i;
	double dMeanVal = 0.0;
	double dSecondVal = 0;
	double dFourthVal = 0;
	double dTmpVal = 0;
	double dFourthParam = 0.0;

	for (i=0;i<len;i++) 
	{	
		dMeanVal += pInput[i];		
	}
	dMeanVal = len/dMeanVal;	

	for (i=0;i<len;i++)
	{
		dTmpVal = pInput[i]*dMeanVal - 1;
		dSecondVal += dTmpVal*dTmpVal;
		dTmpVal = dTmpVal*dTmpVal;
		dFourthVal += dTmpVal*dTmpVal;
	}

	dSecondVal /= len;
	dFourthVal /= len;

	dFourthParam = dFourthVal/dSecondVal/dSecondVal;

	return(dFourthParam);
}

// 对信号功率谱进行参数提取 //
void CSignalParamProbe::GetSpectraParam(double *pSpecParam,double *pSignalPsd,DWORD dwPSDLen)
{
	DWORD i,k,dwMaxIndex;
	double dMaxValue,dTmpValue;

	double dLvlThreshold = 6.0;	

	dMaxValue = 0;
	dTmpValue = 0;
	dwMaxIndex = 0;

	for (i=0;i<dwPSDLen;i++)
	{
		pSignalPsd[i] = log10(pSignalPsd[i]);
	}
	GetSpectraPeaksNum(pSpecParam,pSignalPsd,dwPSDLen);

	dwMaxIndex = GetMaxIndex(pSignalPsd,dwPSDLen,dMaxValue);	
	dTmpValue = -1000.0;

	for ( i = dwMaxIndex-300; i<dwMaxIndex+300; i++)
	{
		if (i >= dwMaxIndex+6 || i <= dwMaxIndex-6)
		{
			if (i >= dwPSDLen)
			{
				k = i-dwPSDLen;   
			}
			else if (i<0) 
			{
				k = i+dwPSDLen;
			}
			else 
			{
				k = i; 
			}
			if (pSignalPsd[k] > dTmpValue) 
			{
				dTmpValue = pSignalPsd[k];
			}
		} 
	}	

	if (pSpecParam[0] == 2 && dwMaxIndex != (DWORD)pSpecParam[1])
	{
		pSpecParam[2] = pSpecParam[1];
		pSpecParam[1] = dwMaxIndex;
	}	
	else
	{
		pSpecParam[1] = dwMaxIndex;
	}

	pSpecParam[3] = dMaxValue - dTmpValue;	
	pSpecParam[4] = GetLvlBandwidth(pSignalPsd, dwPSDLen, dMaxValue - dLvlThreshold); 	
	pSpecParam[5] = CalSpectraCenterFreq(pSignalPsd,dwPSDLen);	
}     

LONG CSignalParamProbe::GetLvlBandwidth(double *pSignalPsd, DWORD dwPSDLen, double dLvlThreshold)
{
	DWORD i,j;

	DWORD dwBW = DWORD(dwPSDLen*m_dPasswidth/m_dSamplingFreq + .5);

	for (i=(dwPSDLen - dwBW)/2; i<(dwPSDLen + dwBW)/2; i++)
	{
		if (pSignalPsd[i]>dLvlThreshold && pSignalPsd[i]>pSignalPsd[i-1]) 
		{
			break;
		}
	}
	for (j = (dwPSDLen + dwBW)/2; j>(dwPSDLen - dwBW)/2; j--)
	{
		if (pSignalPsd[j]>dLvlThreshold && pSignalPsd[j]>pSignalPsd[j+1]) 
		{
			break;
		}
	}

	return(j-i+1);
}


void CSignalParamProbe::GetSpectraPeaksNum(double *pSpecParam,double *pSignalPsd,DWORD dwPSDLen)
{
	DWORD i,dwFrom,dwTo;
	
	DWORD dwPeakNum = 0;
	DWORD dwBW = DWORD(dwPSDLen*m_dPasswidth/m_dSamplingFreq + .5);

	dwFrom = (dwPSDLen - dwBW)/2;
	dwTo   = (dwPSDLen + dwBW)/2;

	double dMaxVal = *(pSignalPsd + dwFrom);
	double dMinVal = *(pSignalPsd + dwFrom);
	for (i = dwFrom; i<dwTo; i++)
	{
		dMaxVal = MAX(pSignalPsd[i],dMaxVal);		
	}	
	for (i = dwFrom; i<dwTo; i++)
	{
		dMinVal = MIN(pSignalPsd[i],dMinVal);		
	}	
	double dLvlGate = dMinVal + 0.9*(dMaxVal-dMinVal);

	for (i = dwFrom; i < dwTo; i++)
	{		
		if ( pSignalPsd[i] > dLvlGate && pSignalPsd[i] > pSignalPsd[i-1] && pSignalPsd[i] > pSignalPsd[i-2] && 
			pSignalPsd[i] > pSignalPsd[i-3] && pSignalPsd[i] > pSignalPsd[i-4] && pSignalPsd[i] > pSignalPsd[i-5] && 
			pSignalPsd[i] > pSignalPsd[i-6] && pSignalPsd[i] > pSignalPsd[i+1] && pSignalPsd[i] > pSignalPsd[i+2] &&
			pSignalPsd[i] > pSignalPsd[i+3] && pSignalPsd[i] > pSignalPsd[i+4] && pSignalPsd[i] > pSignalPsd[i+5] && 
			pSignalPsd[i] > pSignalPsd[i+6])
		{		
			dwPeakNum ++;
			if (dwPeakNum<3)
			{
				pSpecParam[dwPeakNum] = i;
			}
		}
	}

	pSpecParam[0] = dwPeakNum;
}

double CSignalParamProbe::CalSpectraCenterFreq(double* pInput,int N)
{
	double dCenterFreq = 0;	

	DWORD i = 0;

	double dCorr = 0;
	double dLevl = 0;
	DWORD dwBW = DWORD(N*m_dPasswidth/m_dSamplingFreq + .5);

	DWORD dwFrom = (N - dwBW)/2;
	DWORD dwTo   = (N + dwBW)/2;

	double dMaxVal = *(pInput + dwFrom);
	double dMinVal = *(pInput + dwFrom);
	for (i = dwFrom; i<dwTo; i++)
	{
		dMaxVal = MAX(pInput[i],dMaxVal);		
	}	
	for (i = dwFrom; i<dwTo; i++)
	{
		dMinVal = MIN(pInput[i],dMinVal);		
	}	
	double dLvlGate = dMinVal + 0.4*(dMaxVal-dMinVal);

	DWORD dwST = dwFrom;
	DWORD dwED = dwTo;

	for (i = dwFrom; i < dwTo; i++)
	{
		if(pInput[i]>dLvlGate)
		{
			dwST = i;
			break;
		}
	}
	for (i = dwTo; i > dwFrom; i--)
	{
		if(pInput[i]>dLvlGate)
		{
			dwED = i;
			break;
		}
	}
	for ( i = dwST; i < dwED; i++)
	{
		dCorr += (pInput[i]-dLvlGate)*(i-dwST);
		dLevl += (pInput[i]-dLvlGate);
	}
	dCenterFreq = dCorr/dLevl + dwST - 0.5*m_dwFFTSize;	

	return(dCenterFreq);
}

// symmetrical
double CSignalParamProbe::GetSpectraSymmetParam(double *pSignalPsd,DWORD dwPSDLen)
{
	double SymmetParam = 0;

	DWORD i,k;

	double dLeftAverVal = 0;
	double dRightAverVal = 0;

	DWORD dwProtectLen = 4;
	DWORD dwAverLen = 15*m_dwFFTSize/1024;

	double dTmpValue = 0;

	double dMaxValue = 0;	
	DWORD  dwMaxIndex = 0;

	dwMaxIndex = GetMaxIndex(pSignalPsd,dwPSDLen,dMaxValue);	

	k = dwMaxIndex - dwProtectLen - dwAverLen -1;
	for (i = k; i<= dwMaxIndex - dwProtectLen; i++ )
	{
		dLeftAverVal += pSignalPsd[i]*pSignalPsd[i];
	}
	dLeftAverVal /= dwAverLen;

	k = dwMaxIndex + dwProtectLen + dwAverLen +1;
	for (i = dwMaxIndex + dwProtectLen; i< k; i++ )
	{
		dRightAverVal += pSignalPsd[i]*pSignalPsd[i];
	}
	dRightAverVal /= dwAverLen;

	dTmpValue = (dRightAverVal+dLeftAverVal);
	SymmetParam = (dLeftAverVal - dRightAverVal)/dTmpValue;	

	return (SymmetParam);
}

//////////////////////////////////////////////////////////////////////////

double CSignalParamProbe::CalASKSymbolRate( double *pDataIQ,DWORD dwDataLen )
{
	double dSymbolRate = 0.0;	

	DWORD i;

	memset(m_pTmpBufI,0,m_dwSignLen*sizeof(double)/2);
	memset(m_pTmpBufQ,0,m_dwSignLen*sizeof(double)/2);
	memset(m_pSignPsd,0,m_dwFFTSize*sizeof(double));


	////////////////////////////////////////////////////////////////////////
	// 幅度差分谱
	CalSingalEnvelope(pDataIQ,dwDataLen,m_pTmpBufI);

	double dMeanVal = 0;
	for(i = 0;i < dwDataLen/2; i++)
	{
		dMeanVal += m_pTmpBufI[i];
	}	
	dMeanVal /= dwDataLen/2;

	for(i = dwDataLen/2-1;i>0;i--)
	{
		m_pTmpBufQ[i] = fabs(m_pTmpBufI[i] - dMeanVal) - fabs(m_pTmpBufI[i-1] - dMeanVal);
	}	
	m_pTmpBufQ[0] = 0;

	m_pFFTAD->InputData(m_pTmpBufQ,dwDataLen/2);

	DWORD dwSpecLen = 0;
	double*pSpecBuf = NULL;
	
	pSpecBuf = m_pFFTAD->GetSpectrum(dwSpecLen);

	if (pSpecBuf != NULL)
	{
        memmove(m_pSignPsd,pSpecBuf,dwSpecLen*sizeof(double));

		////求信号功率谱最大值////
		int nStartPos = m_dwFFTSize/32;
		int	nMaxIndex = nStartPos;
		double	dPsdMax = m_pSignPsd[nMaxIndex];

		for(i = nStartPos; i < m_dwFFTSize/2; i++)
		{
			if(dPsdMax<m_pSignPsd[i] && m_pSignPsd[i-1]<m_pSignPsd[i] && m_pSignPsd[i+1]<m_pSignPsd[i])
			{
				dPsdMax = m_pSignPsd[i];
				nMaxIndex = i;
			}
		}

		dSymbolRate = nMaxIndex;
	}		

	return(dSymbolRate);
}


void CSignalParamProbe::CalFSKCFDF_Spectra(double* pParam,double *pData,DWORD dwDataLen)
{
	double dFreqDev = 0.0;
	double dCenterFreq = 0.0;

	DWORD i,dwIndex1,dwIndex2;
	double dMaxVal;

	// 信号平方谱分析
	CalSignalSquare(pData,dwDataLen);
	// 信号平方谱分析
	CalAverPowerSpecrum(m_pSignPsd,pData,dwDataLen);

	dwIndex1 = m_dwFFTSize/32;
	dMaxVal = m_pSignPsd[m_dwFFTSize/32];
	for (i=m_dwFFTSize/32; i< m_dwFFTSize-m_dwFFTSize/32;i++)
	{
		if (dMaxVal < m_pSignPsd[i])
		{
			dMaxVal = m_pSignPsd[i];
			dwIndex1 = i;
		}
	}

	if (dwIndex1 < m_dwFFTSize/2)
	{
		dwIndex2 = m_dwFFTSize/2;
		dMaxVal = m_pSignPsd[m_dwFFTSize/2];
		for (i=m_dwFFTSize/2; i< m_dwFFTSize-m_dwFFTSize/32;i++)
		{
			if (dMaxVal < m_pSignPsd[i])
			{
				dMaxVal = m_pSignPsd[i];
				dwIndex2 = i;
			}
		}
	}
	else
	{
		dwIndex2 = m_dwFFTSize/32;
		dMaxVal = m_pSignPsd[m_dwFFTSize/32];
		for (i=m_dwFFTSize/32; i< m_dwFFTSize/2;i++)
		{
			if (dMaxVal < m_pSignPsd[i])
			{
				dMaxVal = m_pSignPsd[i];
				dwIndex2 = i;
			}
		}
	}

	dCenterFreq = dwIndex1 + dwIndex2;
	dCenterFreq = 0.125*( 0.5*dCenterFreq - 0.5*m_dwFFTSize);
	dFreqDev = dwIndex1 > dwIndex2 ? dwIndex1-dwIndex2 : dwIndex2-dwIndex1;

	pParam[0] = dCenterFreq;
	pParam[1] = dFreqDev/4;
}

void CSignalParamProbe::CalFSKCFDF_InstFrequ(double* pParam,double *pData,DWORD dwDataLen)
{
	double dFreqDev = 0.0;
	double dCenterFreq = 0.0;	

	DWORD i,dwFrom,dwTo,K;

	CalSingalFrequency(pData,dwDataLen,m_pFreqBuff);	
	/////////////////////////////////////////
	// 求非弱信号的瞬时频率均值 //
	dwFrom = dwDataLen/8;
	dwTo   = dwFrom + dwDataLen/4;
	double dMaxFreq = m_pFreqBuff[dwFrom];
	double dMinFreq = m_pFreqBuff[dwFrom];
	double dMeanFreq = 0.0;
	for(i = dwFrom ;i<dwTo;i++)
	{
		dMeanFreq += m_pFreqBuff[i];

		if (dMaxFreq < m_pFreqBuff[i])
		{
			dMaxFreq = m_pFreqBuff[i];
		}
		if (dMinFreq > m_pFreqBuff[i])
		{
			dMinFreq = m_pFreqBuff[i];
		}
	}
	dMeanFreq /= dwDataLen/4;
	dCenterFreq = dMeanFreq*m_dwFFTSize/2/PI; 

	// 统计非弱信号的瞬时频率分布 //
	SHORT N = 1000;
	double dFactor = N/(dMaxFreq - dMinFreq);
	memset(m_pTmpBufI,0,m_dwSignLen*sizeof(double)/2);

	for(i = dwFrom ;i<dwTo;i++)
	{
		K = (SHORT)(dFactor*(m_pFreqBuff[i] - dMinFreq));

		m_pTmpBufI[K] += 1.0;
	}	

	// 求分布的最大值 //
	SHORT kn,nIdex1,nIdex2;
	double dMaxVal1,dMaxVal2;

	for(kn = 1; kn < N; kn++)
	{
		m_pTmpBufI[kn] = (m_pTmpBufI[kn] + m_pTmpBufI[kn-1])/2;	
	}

	nIdex1 = 3*N/4;
	dMaxVal1 = m_pTmpBufI[nIdex1];
	for (kn = 3*N/4;kn < N;kn++)
	{
		if (dMaxVal1 < m_pTmpBufI[kn])
		{
			dMaxVal1 = m_pTmpBufI[kn];

			nIdex1 = kn;
		}
	}

	nIdex2 = 0;
	dMaxVal2 = m_pTmpBufI[0];
	for (kn = 0;kn < N/4;kn++)
	{
		if (dMaxVal2 < m_pTmpBufI[kn])
		{
			dMaxVal2 = m_pTmpBufI[kn];

			nIdex2 = kn;
		}
	}

	// 频偏 //
	dFreqDev = (nIdex1 - nIdex2)/dFactor;
	dFreqDev  *= m_dwFFTSize/2/PI/2;

	pParam[0] = dCenterFreq;
	pParam[1] = dFreqDev;
}

double CSignalParamProbe::CalFSKSymbolRate(double *pData,DWORD dwDataLen) 
{
	double dSymbolRate = 0.0;	

	DWORD i;

	memset(m_pTmpBufI,0,m_dwSignLen*sizeof(double)/2);
	memset(m_pTmpBufQ,0,m_dwSignLen*sizeof(double)/2);
	memset(m_pSignPsd,0,m_dwFFTSize*sizeof(double));

	////////////////////////////////////////////////////////////////////////
	// 频率差分谱
	CalSingalFrequency(pData,dwDataLen,m_pTmpBufI);

	double dMeanVal = 0;
	for(i = 0;i < dwDataLen/2; i++)
	{
		dMeanVal += m_pTmpBufI[i];
	}	
	dMeanVal /= dwDataLen/2;

	for(i = dwDataLen/2-1;i>0;i--)
	{
		m_pTmpBufQ[i] = (m_pTmpBufI[i] - dMeanVal) * (m_pTmpBufI[i-1] - dMeanVal);
	}	
	m_pTmpBufQ[0] = 0;

	for(i = dwDataLen/2-1;i>0;i--)
	{
		m_pTmpBufQ[i] = fabs(m_pTmpBufQ[i]) - fabs(m_pTmpBufQ[i-1]);
	}	
	m_pTmpBufQ[0] = 0;

	m_pFFTAD->InputData(m_pTmpBufQ,dwDataLen/2);

	DWORD dwSpecLen = 0;
	double*pSpecBuf = NULL;

	pSpecBuf = m_pFFTAD->GetSpectrum(dwSpecLen);

	if (pSpecBuf != NULL)
	{
        memmove(m_pSignPsd,pSpecBuf,dwSpecLen*sizeof(double));

		LONG i,ki,nCounter,nNum;
		int nFindLen,nLen,nMaxIndex,Indx[2] = {0};
		double dTempValue,dMaxValue;
		double dNormal2,dNormal3;
		double dMax1,dMax2,dMax3;

		int nBandwidth = m_dwFFTSize/64;

		memset(m_pBuffer1,0,m_dwFFTSize*sizeof(double));	
		memset(m_pBuffer2,0,m_dwFFTSize*sizeof(double));
		memset(m_pBuffer3,0,m_dwFFTSize*sizeof(double));
		memset(m_pPsdInex,0,m_dwFFTSize*sizeof(LONG));

		dTempValue = 0.0;
		nFindLen = 0;
		nLen = m_dwFFTSize - nBandwidth;
		for(i = nBandwidth; i < nLen; i++)
		{
			if(m_pSignPsd[i]>m_pSignPsd[i+1] && m_pSignPsd[i]>m_pSignPsd[i-1])
			{
				if(m_pSignPsd[i-1]>m_pSignPsd[i+1])
				{
					m_pBuffer1[nFindLen] = m_pSignPsd[i]+m_pSignPsd[i-1];
				}
				else
				{
					m_pBuffer1[nFindLen] = m_pSignPsd[i]+m_pSignPsd[i+1];
				}

				dTempValue=0.0;
				for(ki = i - nBandwidth; ki < i + nBandwidth+1; ki++)
				{
					dTempValue += m_pSignPsd[ki];
				}

				m_pBuffer2[nFindLen] = (dTempValue-m_pBuffer1[nFindLen])/(2*nBandwidth-1);
				m_pPsdInex[nFindLen] = i;
				nFindLen++;
			}
		}

		for(i=0;i<nFindLen;i++) 
		{
			m_pBuffer3[i] = m_pBuffer1[i]/m_pBuffer2[i];

			if(m_pBuffer1[i]<m_pBuffer2[i])
			{
				m_pBuffer2[i] = 0.0;
			}
			else
			{
				m_pBuffer2[i] = m_pBuffer1[i]-m_pBuffer2[i];
			}
		}

		dMaxValue = GetMaxVal(m_pBuffer3,nFindLen);

		nCounter=0;
		for(i=0;i<nFindLen;i++) 
		{
			if(m_pBuffer3[i]>0.5*dMaxValue)
			{
				m_pBuffer3[nCounter] = m_pBuffer3[i];
				m_pBuffer2[nCounter] = m_pBuffer2[i];
				m_pBuffer1[nCounter] = m_pBuffer1[i];
				m_pPsdInex[nCounter] = m_pPsdInex[i];
				nCounter++;
			}
		}

		dNormal2 = 0.0f;
		dNormal3 = 0.0f;
		for(i=0;i<nCounter;i++)
		{
			dNormal2 += m_pBuffer2[i]*m_pBuffer2[i];
			dNormal3 += m_pBuffer3[i]*m_pBuffer3[i];
		}
		dNormal2 = sqrt(dNormal2);
		dNormal3 = sqrt(dNormal3);

		for(i=0;i<nCounter;i++)
		{		
			m_pTmpBufI[i] = (m_pBuffer2[i]/dNormal2)*(m_pBuffer2[i]/dNormal2);
			m_pTmpBufI[nCounter+i] = (m_pBuffer3[i]/dNormal3)*(m_pBuffer3[i]/dNormal3);
		}

		nMaxIndex = 0;
		dMaxValue = 0.0;
		nMaxIndex = GetMaxIndex(m_pTmpBufI,nCounter,dMaxValue);	

		dMax1 = GetMaxVal(m_pBuffer1,nCounter);
		dMax2 = GetMaxVal(m_pBuffer2,nCounter);
		dMax3 = GetMaxVal(m_pBuffer3,nCounter);

		nNum = 0;
		for(i=0;i<nCounter;i++)
		{
			if(m_pBuffer3[i]>0.65*dMax3)
			{				
				Indx[nNum] = i;
				nNum++;
			}

			if (nNum >= 2)
			{
				break;
			}
		}

		if (nNum == 2 && *(m_pPsdInex + Indx[1]) % *(m_pPsdInex + Indx[0]) == 0)
		{
			dSymbolRate = m_pPsdInex[Indx[0]];
		}
		else
		{
			dSymbolRate = m_pPsdInex[nMaxIndex];
		}
	}			

	return(dSymbolRate);
}

double CSignalParamProbe::CalASKFrequncy( double* pDataIQ, DWORD dwDataLen )
{
	double dCenterFreq = .0;
	
	// 信号幅度归一化 //
	if (!SignalNormalize(pDataIQ,dwDataLen))
	{
		return(ERROR_BUFFER_ALL_ZEROS);
	}
	// Signal Power Spectrum //
	CalAverPowerSpecrum(m_pSignPsd,pDataIQ,dwDataLen);	

	////求信号功率谱最大值////
	int nStartPos = m_dwFFTSize/4;
	int	nMaxIndex = nStartPos;
	double	dPsdMax = m_pSignPsd[nMaxIndex];

	for(DWORD i = nStartPos; i < 3*m_dwFFTSize/4; i++)
	{
		if( dPsdMax < m_pSignPsd[i] && m_pSignPsd[i-1] < m_pSignPsd[i] && m_pSignPsd[i+1] < m_pSignPsd[i] && 
			m_pSignPsd[i-2] < m_pSignPsd[i] && m_pSignPsd[i+2] < m_pSignPsd[i])
		{
			dPsdMax = m_pSignPsd[i];
			nMaxIndex = i;
		}
	}

	dCenterFreq = nMaxIndex - 0.5*m_dwFFTSize;

	return(dCenterFreq);
}

//====================================
// 高阶QAM信号调制样式识别 //
SHORT CSignalParamProbe::QAMRecognition( double* pDataIQ,DWORD dwDataLen,double dSymbolLen )
{
	DWORD dwNum,dwCnt;
	SHORT nSymLen = SHORT(dSymbolLen + 0.5);
	double dCnt = .0;
	double dx = .0, dy = .0;

	ZeroMemory(m_pTmpBufI,m_dwSignLen*sizeof(double)/2);

	for(SHORT k = 0; k < nSymLen; k++)
	{
		dCnt  = 1.0*k;
		dwCnt = 0;
		while(dwCnt < dwDataLen/2)
		{	
			dwCnt = DWORD(dCnt+0.5);
			dx = pDataIQ[2*dwCnt];
			dy = pDataIQ[2*dwCnt+1];
			m_pTmpBufI[k] += sqrt(dx*dx + dy*dy);
			dCnt += dSymbolLen;
		}
	}

	double dMaxVal = .0;
	LONG nIdx = GetMaxIndex(m_pTmpBufI,nSymLen,dMaxVal);

	dwNum = 0;
	dwCnt = 0;
	dCnt = 1.0*nIdx;	
	while(dwCnt < dwDataLen/2)
	{	
		dwCnt = DWORD(dCnt+0.5);
		m_pTmpBufI[dwNum++] = pDataIQ[2*dwCnt];
		m_pTmpBufI[dwNum++] = pDataIQ[2*dwCnt+1];	
		dCnt += dSymbolLen;
	}	

	double dMeanVal = .0;
	for (DWORD n = 0; n < dwNum; n++)
	{
		dMeanVal += m_pTmpBufI[n];
	}
	dMeanVal /= dwNum;

	dMaxVal = .0;
	for (DWORD n = 0; n < dwNum; n++)
	{
		dMaxVal = dMaxVal > (m_pTmpBufI[n]- dMeanVal) ? dMaxVal:(m_pTmpBufI[n]- dMeanVal);
	}

	dCnt = .0;
	for (DWORD n = 0; n < dwNum; n++)
	{
		dx = (m_pTmpBufI[n]- dMeanVal)/dMaxVal;

		dCnt += dx*dx;
	}
	dCnt /= dwNum;



	//FILE* fp1;
	//fp1 = fopen("F:\\peak.bin","wb");
	//fwrite(m_pTmpBufI,sizeof(double),dwNum,fp1);
	//fclose(fp1);

	return 4;
}
