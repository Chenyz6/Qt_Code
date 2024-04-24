#ifndef COMMON_INNER_H
#define COMMON_INNER_H
#include <QtWidgets>
#include <unordered_map>
#include "Common_Predef.h"

using namespace std;

#pragma pack (push,1)
//数据包头
struct NetData_Head
{
    unsigned char		ucHead_1;				//头部标识1		0xA5
    unsigned char		ucHead_2;				//头部表示2		0x5A
    unsigned long  		lLength;				//报文长度
    unsigned short		sInfoType;				//信息类别号

    NetData_Head()
    {
        ucHead_1 =	0xA5;
        ucHead_2 =	0x5A;
        lLength =	0;
        sInfoType = 0;
    }
};

//各模块当前运行状态
struct Inner_Flow_Running_State
{
    bool bParonamaFlow;							//全景流程是否运行
    bool bParonamaPauseFlow;                    //全景流程是否暂停
    bool bAnalyseFlow;							//识别流程是否运行
    bool bDemodFlow;							//解调流程是否运行
    bool bDirFind;								//测向流程是否运行
    Inner_Flow_Running_State()
    {
        bParonamaFlow = false;
        bParonamaPauseFlow = false;
        bAnalyseFlow =	false;
        bDemodFlow =	false;
        bDirFind =		false;
    }
};

//信号类型表(总)
struct DisSgnType
{
    unordered_map<int, string> DisSgnTypeMap;
    DisSgnType()
    {
        DisSgnTypeMap.insert(pair<int, string>(0, "Unknown"));
        DisSgnTypeMap.insert(pair<int, string>(1, "ACARS"));
        DisSgnTypeMap.insert(pair<int, string>(2, "AIS"));
        DisSgnTypeMap.insert(pair<int, string>(3, "AMSAT_P3D"));
        DisSgnTypeMap.insert(pair<int, string>(4, "APCO_P25"));
        DisSgnTypeMap.insert(pair<int, string>(5, "ATIS"));
        DisSgnTypeMap.insert(pair<int, string>(6, "FMS_BOS"));
        DisSgnTypeMap.insert(pair<int, string>(7, "INMARSAT_Aero_p"));
        DisSgnTypeMap.insert(pair<int, string>(8, "MOBITEX_8000"));
        DisSgnTypeMap.insert(pair<int, string>(9, "MPT1327"));
        DisSgnTypeMap.insert(pair<int, string>(10, "NWR_SAME"));
        DisSgnTypeMap.insert(pair<int, string>(11, "POCSAG"));
        DisSgnTypeMap.insert(pair<int, string>(12, "TETRA"));
        DisSgnTypeMap.insert(pair<int, string>(13, "ORBCOMM"));
        DisSgnTypeMap.insert(pair<int, string>(14, "VDL_MODE2"));
        DisSgnTypeMap.insert(pair<int, string>(15, "TADIL_A"));
        DisSgnTypeMap.insert(pair<int, string>(16, "TADIL_C"));
    }

};

//识别解调信号类型表(总)
struct AnalyseSgnType
{
    QVector<QString>	SgnType;				//信号类型
    QVector<char>		SgnID;					//信号映射ID
    AnalyseSgnType()
    {
        SgnType.append("Noise");		SgnID.append(0);
        SgnType.append("CW");			SgnID.append(1);
        SgnType.append("AM");			SgnID.append(2);
        SgnType.append("FM");			SgnID.append(3);
        SgnType.append("2ASK");			SgnID.append(4);
        SgnType.append("2FSK");			SgnID.append(5);
        SgnType.append("4FSK");			SgnID.append(6);
        SgnType.append("8FSK");			SgnID.append(7);
        SgnType.append("MSK");			SgnID.append(8);
        SgnType.append("GMSK");			SgnID.append(9);
        SgnType.append("BPSK");			SgnID.append(10);
        SgnType.append("QPSK");			SgnID.append(11);
        SgnType.append("8PSK");			SgnID.append(12);
        SgnType.append("Pi/2-DQPSK");	SgnID.append(13);
        SgnType.append("Pi/4-DQPSK");	SgnID.append(14);
        SgnType.append("PI/8-DQPSK");	SgnID.append(15);
        SgnType.append("OQPSK");		SgnID.append(16);
        SgnType.append("PSK4S");        SgnID.append(17);
        SgnType.append("8QAM");			SgnID.append(18);
        SgnType.append("16QAM");		SgnID.append(19);
        SgnType.append("32QAM");		SgnID.append(20);
        SgnType.append("64QAM");		SgnID.append(21);
        SgnType.append("16APSK");		SgnID.append(22);
        SgnType.append("32APSK");		SgnID.append(23);
        SgnType.append("USB_Link11");	SgnID.append(24);
        SgnType.append("LSB_Link11");	SgnID.append(25);
        SgnType.append("ISB_Link11");	SgnID.append(26);
        SgnType.append("UV_Link11");	SgnID.append(27);
        SgnType.append("UV_Link4A");	SgnID.append(28);
        SgnType.append("VHF_ACARS");	SgnID.append(29);
        SgnType.append("VHF_AIS");      SgnID.append(30);
        SgnType.append("UV_MPT1327");   SgnID.append(31);
        SgnType.append("FM_FSK2");      SgnID.append(32);
        SgnType.append("FM_BPSK");      SgnID.append(33);
        SgnType.append("FM_QPSK");      SgnID.append(34);
    }
};

//文件属性参数(主界面显示
struct Data_File_Attribute
{
    QString         _fileName;      //文件名
    QString         _fileType;      //文件类型
    unsigned long   lDataSzie;      //文件大小
    Data_File_Attribute()
    {
        _fileName = "";
        _fileType = "";
        lDataSzie = 0;
    }
};

//全景绘图参数设置
struct PanoParamSet
{
    double              xlower;                 //x轴底值
    double              xupper;                 //x轴顶值
    double              ylower;                 //y轴底值
    double              yupper;                 //y轴顶值
    double              x_value;                //鼠标点击的x值
    double              dpressvalue;            //鼠标按下保存的值
    PanoParamSet()
    {
        xlower = 0;
        xupper = 0;
        ylower = 0;
        yupper = 0;
        x_value = 0;
        dpressvalue = 0;
    }
};

//识别选中范围
struct Distingguish_Range
{
    bool bIsDrow;
    QPointF PrsPoint;
    QPointF RlsPoint;
    Distingguish_Range(QPointF prspot, QPointF rlspot):PrsPoint(prspot),RlsPoint(rlspot)
    {
        bIsDrow = true;
    }
    Distingguish_Range()
    {
        bIsDrow = false;
    }
};

//抽取参数
struct ExtractParam
{
    double dSamplizeParam;                      //采样率        //原始采样率 从界面文件参数获取
    double dSignalFreq;                         //信号频率      //框选中心频率
    double dPassWidth;                          //通过带宽      //==输出采样率=框选的采样率宽度
    double dStopWidth;                          //停止带宽      //==通过带宽的1.2倍
    double OutSamplingFreq;                     //输出采样率
    int    nTag;                                //信号结果表格标记
    Distingguish_Range stDistingguish_Range;    //信号位置
    ExtractParam()
    {
        dSamplizeParam = 0;
        dSignalFreq = 0;
        dPassWidth = 0;
        dStopWidth = 0;
        OutSamplingFreq = 0;
        nTag = -1;
    }

};

//主界面数据参数汇总
struct DataParam
{
    //数据文件参数
    char cFileType;                             //文件类型
    char cDataType;                             //数据类型
    char cChannelType;                          //信道类型
    bool bInSpecFlag;                           //反谱标志
    double dSampFreq;                           //采样频率      (KHz
    double dTunFreq;                            //调谐频率      (KHz
    double dCFFreq;                             //中频频率      (KHz
    unsigned int unDataOffset;                  //读取偏移

    //数据分析参数
    char cFFTOperNum;                           //FFT运算点数
    char cFFTWinType;                           //FFT加窗类型
    char cFFTSmoNum;                            //FFT平滑点数
    unsigned int nFFTAverNum;                   //FFT平均次数

    //信号通道参数
    double dCenFreq;                            //中心频率      (KHz
    double dSgnBW_Chan;                         //信号带宽      (KHz
    double dOutputSamp_chan;                    //输出采样      (KHz
    char cSgnType_Chan;                         //信号调制类型

    //信号解调参数
    char cSgnType_modu;                         //信号调制类型
    double dSgnFreq;                            //信号频率      (KHz
    double dSgnBW_modu;                         //信号带宽      (KHz
    double dSecFreq;                            //二次载频      (KHz
    double dSymRate;                            //符号速率      (KBd
    double dFreqOffset;                         //调制频偏      (KHz
    double dOutputSamp_modu;                    //输出采样      (KHz
    bool bBitDiffcode;                          //比特差分编码
    DataParam()
    {
        cFileType =         0;
        cDataType =         1;
        cChannelType =      1;
        bInSpecFlag =       0;
        dSampFreq =         32.000;
        dTunFreq =          0.000;
        dCFFreq =           0.000;
        unDataOffset =      0;

        cFFTOperNum =       4;
        cFFTWinType =       1;
        cFFTSmoNum =        7;
        nFFTAverNum =       8;

        dCenFreq =          0.000;
        dSgnBW_Chan =       0.000;
        dOutputSamp_chan =  0.000;
        cSgnType_Chan =     0;

        cSgnType_modu =     0;
        dSgnFreq =          0.000;
        dSgnBW_modu =       0.000;
        dSecFreq =          0.000;
        dSymRate =          0.000;
        dFreqOffset =       0.000;
        dOutputSamp_modu =  0.000;
        bBitDiffcode =      0;
    }
    bool operator == (const DataParam & a)const
    {
        return (cFileType == a.cFileType&&
        cDataType == a.cDataType&&
        cChannelType == a.cChannelType&&
        bInSpecFlag == a.bInSpecFlag&&
        dSampFreq == a.dSampFreq&&
        dTunFreq == a.dTunFreq&&
        dCFFreq == a.dCFFreq&&
        unDataOffset == a.unDataOffset&&

        //数据分析参数
        cFFTOperNum == a.cFFTOperNum&&
        cFFTWinType == a.cFFTWinType&&
        cFFTSmoNum == a.cFFTSmoNum&&
        nFFTAverNum == a.nFFTAverNum&&

        //信号通道参数
        dCenFreq == a.dCenFreq&&
        dSgnBW_Chan == a.dSgnBW_Chan&&
        dOutputSamp_chan == a.dOutputSamp_chan&&
        cSgnType_Chan == a.cSgnType_Chan&&

        //信号解调参数
        cSgnType_modu == a.cSgnType_modu&&
        dSgnFreq == a.dSgnFreq&&
        dSgnBW_modu == a.dSgnBW_modu&&
        dSecFreq == a.dSecFreq&&
        dSymRate == a.dSymRate&&
        dFreqOffset == a.dFreqOffset&&
        dOutputSamp_modu == a.dOutputSamp_modu&&
        bBitDiffcode == a.bBitDiffcode);
    }
};

//配置文件信息
struct IniFileMsg
{
    QString DatFilePath;                        //文件复选框路径
    char cCurMDIChildWind;                      //当前子窗口
    DataParam stDataParam;                      //全局UI参数
    IniFileMsg()
    {
        DatFilePath = "";
        cCurMDIChildWind = 0;
    }
};

//数据文件保存(uint8_t, uint16_t, uint32_t)
struct DataSave
{
    char*   uint8_Data;
    short*  uint16_Data;
    int*    uint32_Data;
    double* dData;
    long*   lData;
    unsigned int nDataLen;                      //数据点数:与字节长度无关
    bool    Reset() //重置数据
    {
        if(nDataLen == 0)
            return false;
        if(uint8_Data != nullptr)
        {
            delete[] uint8_Data;
            uint8_Data = nullptr;
        }
        if(uint16_Data != nullptr)
        {
            delete[] uint16_Data;
            uint16_Data = nullptr;
        }
        if(uint32_Data != nullptr)
        {
            delete[] uint32_Data;
            uint32_Data = nullptr;
        }
        if(dData != nullptr)
        {
            delete[] dData;
            dData = nullptr;
        }
        if(lData != nullptr)
        {
            delete[] lData;
            lData = nullptr;
        }
        nDataLen = 0;
        return true;
    }
    DataSave()
    {
        uint8_Data = nullptr;
        uint16_Data = nullptr;
        uint32_Data = nullptr;
        dData = nullptr;
        lData = nullptr;
    }
};

//识别信号数据保存
struct SgnAnalysisResult
{
    double dConfidence;                         //置信度
    double dSgnFreq;                            //信号频率
    double dSgnBW;                              //信号带宽
    int    nSgnType;                            //识别类型
    int    nModuType;                           //调制类型
    int    nModu_Sec;                           //二次调制
    int    nTransType;                          //传输类型
    double dSymRate;                            //信号速率
    int    nOffset;                             //调制频偏
    double dSgnNoiseRate;                       //信噪比
    int    nTag;                                //信号结果表格标记
    Distingguish_Range stDistingguish_Range;
    SgnAnalysisResult()
    {
        dConfidence = 0;
        dSgnFreq = 0;
        dSgnBW = 0;
        nSgnType = 0;
        nModuType = 0;
        nModu_Sec = 0;
        nTransType = 0;
        dSymRate = 0;
        nOffset = 0;
        dSgnNoiseRate = 0;
        nTag = -1;
    }
};

/************************************************************************/
/*                            参数下发数据结构                          */
/************************************************************************/
//全景控制指令
struct Param_Panorama
{
    unsigned long		ulStartFreq;			//起始频率
    unsigned long		ulEndFreq;				//终止频率
    int					nResolution;			//分辨率		//0:0.025	1:0.0125
    Param_Panorama()
    {
        ulStartFreq =	0;
        ulEndFreq =		0;
        nResolution =	0;
    }
};

//识别控制指令
struct Param_Distinguish
{
    char				cChannelNo;				//通道号
    double				dCenterFreq;			//中心频率        //KHz
    short				sFFTSize;				//FFT点数
    unsigned char		ucFFTMvSize;			//FFT滑动点数
    unsigned char		ucFFTNum;				//FFT平均次数
    int					nWinFunc;				//窗函数         //0:Boxcar  1:Hanning  2:Hamming  3:Blackman  4:Blackman_Harris
    bool				bComSignalFlag;			//复信号标志
    bool				bInSpecFlag;			//反谱标志
    bool				bDBFlag;				//DB标志
    unsigned char		ucSampRate;				//采样率         //KHz
    double				dsignDW;				//信号带宽		//KHz
    double				dsignCenFreq;			//信号中心频率	//Hz
    Param_Distinguish()
    {
        cChannelNo =		0;
        dCenterFreq =		0;
        sFFTSize =			0;
        ucFFTMvSize =		0;
        nWinFunc =			0;
        ucFFTNum =			0;
        bComSignalFlag =	false;
        bInSpecFlag =		false;
        bDBFlag =			false;
        ucSampRate =		0;
        dsignDW =			0;
        dsignCenFreq =		0;
    }
};

//解调控制指令
struct Param_RegularDemod
{
    double				dCenterFreq;			//中心频率		//KHz
    unsigned char		ucSampRate;				//采样率
    double				dSignBW;				//信号带宽		//KHz
    double				dSignFreq;				//信号频率		//Hz
    bool				bIQsignal;				//复信号标志
    bool				bFPsignal;				//反谱标志
    double				dBaudRate;				//波特率		//sym/s
    unsigned char		ucModType;				//解调类型		//信号类型查看表：AnalyseSgnType
    bool				bDeCode;				//差分编码标志
    Param_RegularDemod()
    {
        dCenterFreq =	0;
        ucSampRate =	0;
        dSignBW =		0;
        dSignFreq =		0;
        bIQsignal =		false;
        bFPsignal =		false;
        dBaudRate =		0;
        ucModType =		0;
        bDeCode =		false;
    }
};

//窄带测向控制指令
struct Param_NBandDF
{
    double			    dFreq;					//频率
    int					nWorkMode;				//工作模式
    long				lDF_1_RFA;				//测向1射频衰减
    long				lDF_1_MFA;				//测向1中频衰减
    long				lDF_2_RFA;				//测向2射频衰减
    long				lDF_2_MFA;				//测向2中频衰减
    Param_NBandDF()
    {
        dFreq =		100;
        lDF_1_RFA = 0;
        lDF_1_MFA = 0;
        lDF_2_RFA = 0;
        lDF_2_MFA = 0;
        nWorkMode = 0;
    }
};


/************************************************************************/
/*                            数据回执数据结构                          */
/************************************************************************/
//上报全景数据
struct Outer_RepPano
{
    long			lStartFreq;					//起始频率
    long			lEndFreq;					//终止频率
    unsigned long	ulLineNumber;				//谱线数
    Outer_RepPano()
    {
        lStartFreq =	0;
        lEndFreq =		0;
        ulLineNumber =	3200;
    }
};

//上报识别结果
class Outer_RepDistinguish
{
public:
    unsigned char      ucChannelNo;				//通道号
    double             dAnalyseFreq;			//分析频率			//MHz
    double             dCenterFreq;				//中心频率			//MHz
    unsigned char      ucSignalLevel;			//信号电平
    double             dFreqBW;					//频率带宽:			//KHz
    unsigned long      ulIFDataLength;			//中频频谱数据长度	//字节
    short              sIFData[MAX_IFDATA_LENGTH];//中频频谱数据
    unsigned short     usSignalType;			//信号样式
    unsigned char      ucIdentifyCon;			//识别可信度
    unsigned long      ulCodeRate;				//码速率
    Outer_RepDistinguish()
    {
        ucChannelNo =		0;
        dAnalyseFreq =		100.0;
        dCenterFreq =		100.0;
        ucSignalLevel =		0;
        dFreqBW =			0;
        ulIFDataLength =	1;
        memset(sIFData, 0, sizeof(short)*MAX_IFDATA_LENGTH);
        usSignalType =		0;
        ucIdentifyCon =		0;
        ulCodeRate =		0;
    }
};

//上报解调结果
struct Outer_RepRegularDemod
{
    double             dDemodFreq;				//解调频率			//MHz
    unsigned short     usDemodType;				//解调信号样式
    unsigned long      ulDemodBW;				//解调带宽
    unsigned short     usDemodDataLength;		//解调数据长度		//字节
    long*              lDemodData;//解调数据

    Outer_RepRegularDemod()
    {
        dDemodFreq =		0;
        usDemodType =		0;
        ulDemodBW =			0;
        usDemodDataLength = 0;
        lDemodData = NULL;
    }
};

//上报点频测向结果
struct Outer_RepNBandDF
{
    double				dfreq;                  //频率
    unsigned char       cAmpLevel;              //幅度
    short				sAzimuth;               //方位角
    Outer_RepNBandDF()
    {
        dfreq =		100;
        cAmpLevel = 0;
        sAzimuth =	0;
    }
};



//注册自定义信号槽传输类型
class RegisterMetaType
{
  public:
    RegisterMetaType()
    {
        qRegisterMetaType<Data_File_Attribute>("Data_File_Attribute");
        qRegisterMetaType<SgnAnalysisResult>("SgnAnalysisResult");
        qRegisterMetaType<SgnAnalysisResult>("ExtractParam");
        qRegisterMetaType<SgnAnalysisResult>("Outer_RepRegularDemod");
    }
};
#pragma pack (pop)
#endif // COMMON_INNER_H
