//////////////////////////////////////////////////////////////////////////
//	SgnlPrcsDll.h
//
//////////////////////////////////////////////////////////////////////////
#pragma once
#pragma pack(1)
#ifndef _SGNLPRCSDLL_H_
#define _SGNLPRCSDLL_H_

#include <stdio.h>
//#include <tchar.h>
//#include<minwindef.h>
#include"type.h"
#include "CmnHdr.h"
//#include ".\fftw3.h"
#include "OfflineError.h"
//#pragma comment (lib,"./SgnlPrcsDll.lib")

//#include "resource.h"       // 主符号


/////////////////////// 信号处理模块加载参数 ////////////////////////////

// 配置文件路径名 //
//extern  TCHAR g_pzdllSetupFile[];
// 临时文件路径 //
//extern  TCHAR g_pzdllTmpFilePath[];

// 系统加载信号调制样式种类 //
extern  short  g_ndllModuTypeNum;
extern  short*  const g_pdllSysModuType;
//extern  TCHAR** CONST g_pdllSysModuName;

// 系统加载跳频信号调制样式种类 //
extern  short  g_ndllFHModuNum;
extern  short*  const g_pdllFHModuType;

// 可加载数据通道个数 //
extern  short g_dllDataChanNum;
// 可加载信号通道个数 //
extern  short g_dllSignalChanNum;

// 滤波器设计类型 //
extern  short  g_dllLPFType ;
// 阻带/通带因子 //
extern  double g_dllStopwidthFactor;
// 通带波纹 lg//
extern  double g_dllPassRipple;
// 阻带衰减 dB//
extern  double g_dllStopRipple;
// 解调因子 //
extern  double g_dllDemodFactor;
// DDC最大抽取倍数 //
extern  double g_dllExtractMax;


/////////////////////////////信号处理模块常量/////////////////////////////
// 文件路径名最大长度 //
const short OSA_MAX_PATH = 8*260;//MAX_PATH=260

const double PI = 3.1415926535897932384626433832795;

const double RATIO_MIN = 3.5;	// dSamplingFreq/dPassWidth
const double RATIO_MAX = 8.0;

// 可增加信号通道数目最大值 //
const short CHANNEL_NUM_MAX = 10;

const double PASS_RIPPLE_MIN = 0.00001;
const double PASS_RIPPLE_MAX = 2;

const double STOP_RIPPLE_MIN = 20;
const double STOP_RIPPLE_MAX = 100;

// 信号最小检测带宽 //
const short DETECTION_BANDWIDTH_MIN = 5;

// 猝发信号空闲时段解调器输出零的个数 //
const short UCV_DEM_ZEROS_NUM = 16;
// 信号检测最大个数 //
const short UCV_SIGNAL_NUM_MAX = 16;

const short UVFH_SPCTRA_NUM_MIN = 8;
const short UVFH_SPCTRA_NUM_MAX = 100;
const short UVFH_BANDWIDTH_MIN = 5;
const short UVFH_SIGNAL_NUM_MAX = 64;
const short UVFH_NET_NUM_MAX = 8;
const short UVFH_FREQ_SET_MAX = 1024;

const double UVFH_NET_SORT_PAR = 0.25;
const short  UVFH_NET_SORT_NUM = 4;


// 信号调制类型种类数目最大值
const short SMT_NUM_MAX = 32;

// 信号调制类型 //
enum SignalModulationType
{
	SMT_UNKNOWN = 0,
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
    SMT_16APSK, //4+12
	SMT_USB_LINK11,
	SMT_LSB_LINK11,
	SMT_ISB_LINK11,
	SMT_UHF_LINK11,
    SMT_UHF_LINK4A,
    SMT_256QAM,
};


//FFT加窗类型 //
enum WindowFunction
{
	Boxcar,
	Hanning,
	Hamming,
	Blackman,
	Blackman_Harris,
};

//滤波器设计方法类型 //
enum FIRLPFDesignType
{
	FIRLPFDT_Hanning,
	FIRLPFDT_Hamming,
	FIRLPFDT_Blackman,
	FIRLPFDT_Kaiser,
	FIRLPFDT_Equiripple,
};

// 均衡器类型 //
enum EqualizerType
{
	EQUATYPE_NO,
	EQUATYPE_MCMA,
	EQUATYPE_MDFE
};

//////////////////////////////////////////////////////////////////////////

// 复信号 //
typedef struct{
	double I;
	double Q;
}COMPLEX_SIGNAL;

// 输入文件信息 //
typedef struct {
	short	nFileType;			//文件类型
	short	nDataType;			//数据类型
	long	dwOpenOffset;		//文件读取偏移字节数
	bool	bIsInverse;			//反谱标志
	bool	bIsComplex;			//复数通道标志
	double	dTuneFreq;			//调谐频率 Hz
	double	dIntermediateFreq;	//中频频率 Hz
	double	dSamplingFreq;		//采样频率 Hz
} DATA_FILE_PARAM;

// 数据信道参数 //
typedef struct {
	short	nDataChannelID;		//数据通道唯一标识号
	double	dTuneFreq;			//调谐频率 Hz
	double	dIntermediateFreq;	//中频频率 Hz
	double	dBandwidth;			//信道带宽 Hz
	double	dSamplingFreq;		//采样频率 Hz
	bool	bIsInverse;			//反谱标志
	bool	bIsComplex;			//复数通道标志
}DATA_CHANNEL_PARAM;

// 信号通道参数 //
typedef struct {
	short  nSignalChannelID;	//信号通道唯一标识号
	double dCenterFreq;			//信号频率 Hz
	double dBandwidth;			//通带带宽 Hz
	double dStopWidth;			//阻带带宽 Hz
	double dOutSamplingFreq;	//输出采样频率 Hz
} SIGNAL_CHANNEL_PARAM;

// 功率谱参数 //
typedef struct {
	long	dwFFTSize;	//FFT长度
	long	dwFFTMove;	//FFT平滑点数
	short	nWindType;	//FFT窗函数类型
	short	nAverNum;	//FFT平均次数
} SPECTRUM_PROBE_PARAM;

// 信号解调参数 //
typedef struct {
	short  nSCID;		//信号通道号
	short  nModuType;	//信号调制类型
	double dTimeFrom;	//起始时间 s
	double dDuration;	//持续时间 s
	double dSignalFreq;	//信号频率 Hz
	double dSymbolRate;	//符号速率 kBd
	double dFreqShift;	//频偏 Hz
	double dOutSampling;//输出采样
	double dEstimSNR;	//信噪比 dB
} SIGNAL_DEMOD_PARAM;

// 信号检测结果参数 //
typedef struct{
	short  nSCID;		// 信号通道号
	double dTimeFrom;	// 起始时间 ms
	double dDuration;	// 驻留时间 ms
	double dCenterFreq; // 中心频率Hz
	double dPassWidth;  // 信号带宽Hz
	double dEstimSNR;	// 信噪比 dB
} SIGNAL_DETECT_PARAM;

// 信号参数统计 //
typedef struct {
	long  dwCntSum;	//参数统计总数
	long  dwCntNum;	//当前信号参数出现次数
	short  nSCID;		//信号通道号
	short  nModuType;	//信号调制类型
	double dTimeFrom;	//起始时间 s
	double dDuration;	//持续时间 s
	double dSignalFreq;	//信号频率 Hz
	double dSymbolRate;	//符号速率 kBd
	double dFreqShift;	//频偏 Hz
	double dEstimSNR;	//信噪比 dB
} SIGNAL_STAT_PARAM;


// 跳频分选结果 //
typedef struct{
	short  nNetID;		// 网台号
	short  nFreqNum;	// 频率个数
	double* pFreqSet;	// 频率集 Hz
	double dCycleTime;	// 跳频周期 ms
	double dDuration;	// 驻留时间 ms
	double dPassWidth;	// 信号带宽 Hz
	double dAverLevel;  // 信号平均电平 dB
	double dMinFreqStep; // 最小带宽间隔 Hz
	double dMaxFreqStep; // 最大带宽间隔Hz
	double dSortParam;   // 分选参数
}USFH_NETSORT_RESULT;

// 短时信号检测结果参数 //
typedef struct{
	double dCenterFreq;	// 中心频率Hz
	double dPassWidth;	// 信号带宽Hz
	double dAverLevel;	// 信号电平 dB
	double dTimeFrom;	// 起始时间 ms
	double dTimeStop;	// 结束时间 ms
	double dDuration;	// 驻留时间 ms
	double dCycleTime;	// 跳频周期 ms
	double dSortParam;  // 分选参数
} USTH_SIGNAL_DETECT_PARAM;

// 中频频谱数据参数 //
typedef struct{
	short nChanNum;				// 通道数
	double dIntermediateFreq;	// 中频频率 Hz
	double dBandwidth;			// 信道带宽 Hz
	double dStartFreq;			// 起始频率 Hz
	double dFreqScale;			// 频率分辨率 Hz
	double dTimeScale;			// 时间分辨率 s
	double dLevlScale;			// 量化电平
	long  lSpcLinNum;			// 谱线数量
}SAVE_WTFL_PARAM;

//////////////////////// 信号模块函数 ////////////////////////////////////

// 错误信息显示 //
//extern  void MsgBoxLastErrorShow(long dwErrorID);

// 求最小值 //
extern  long GetMinVal(long* X,int N);
extern  double GetMinVal(double* X,int N);

// 求最大值 //
extern  long GetMaxVal(long* X,int N);
extern  double GetMaxVal(double* X,int N);
extern  long GetMaxIndex(double* X, long N, double& dMaxValue);

// 信号归一化 //
extern  bool SignalNormalize(double* pData,long dwDataLen);
extern  bool SignalNormalize(double* pData,unsigned long dwDataLen);
extern  bool SignalNormalize(long* pData,double* pSigIQ,long dwDataLen);


// 计算信号平方 //
extern  void CalSignalSquare(double *pData, long lDataLen);
extern  void CalSignalSquare(double *pDataI, double *pDataQ, long lDataLen);


// 获取信号瞬时包络 //
extern  void CalSingalEnvelope(double *pData, long lDataLen, double* pResult);
// 获取信号瞬时频率 //
extern  void CalSingalFrequency(double* pData,long nDataLen,double* pResult);
// 获取信号瞬时相位 //
extern  void CalSingalPhase(double* pData,long nDataLen,double* pResult);

// 获取信号频率 //
extern  double GetSignalFreq(double dSignalFreq,DATA_CHANNEL_PARAM* pChanPara,bool bInv = true);
// 获取信号数字频率 //
extern  double GetDigitalFreq(double dSignalFreq,DATA_CHANNEL_PARAM* pChanPara,bool bInv = true);
// 二次调制的AM解调//
extern double* Twice_AM(double* data, long size);
//实信号变为复信号的下变频
extern  double* ddc(double *sign, double fs, double fc, long size);
//ACARS信号AM预先解调
extern double* ACARS_AM(double *sign, long size);
//希尔伯特变换


#endif //#ifndef _SGNLPRCSDLL_H_

//////////////////////////// End of File /////////////////////////////////
