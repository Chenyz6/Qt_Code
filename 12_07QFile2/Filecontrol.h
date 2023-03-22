#ifndef FILECONTROL_H
#define FILECONTROL_H
#include <QtWidgets>
#include <QFileDialog>
#include <QDebug>
#include <QThreadPool>
#include <QtEndian>
#include <QWaitCondition>
#include "Common_Inner.h"
#include "Common_Predef.h"
#include "Global.h"



//解决中文乱码,添加后不再使用QString::fromLocal8Bit(" ")修饰
#pragma execution_character_set("utf-8")

class Filecontrol:public QObject
{
    Q_OBJECT
public:
    Filecontrol(QObject *parent = nullptr);
    ~Filecontrol();

public:
    void onSelected();  //选中文件
    void Inner_ModuleDataRouter(char* cCmdContent, int nDataLength, unsigned short usInfoType, unsigned short usDestID);//内部数据路由
private:
    QFileDialog* FileCtrl;
    QString dir = " ";                //指定文件选择对话框默认打开的目录, ".":程序运行目录, "/":当前盘符根目录
    QString _file_path;         //当前选取文件的路径
    QStringList _file_history;  //文件浏览历史记录
    QStringList _strlist;       //文件类型
private:
    void Init();
    bool GetFileData(Data_File_Attribute& stData_File_Attribute);
    void GetIniFileMsg();       //获取配置文件信息
    void SetFileMsg();          //将参数保存到配置文件和全局参数

private:
    QLabel* lab_FileType =      new QLabel("文件类型:");
    QLabel* lab_DataType =      new QLabel("数据类型:");
    QLabel* lab_ChannelType =   new QLabel("信道类型:");
    QLabel* lab_InTab =         new QLabel("反谱标志:");
    QComboBox* cBx_FileType =   new QComboBox();
    QComboBox* cBx_DataType =   new QComboBox();
    QComboBox* cBx_ChannelType = new QComboBox();
    QComboBox* cBx_InSpecFlag = new QComboBox();
    QLabel* lab_SampRate =      new QLabel("采样频率[KHz]:");
    QLabel* lab_TunFreq =       new QLabel("调谐频率[KHz]:");
    QLabel* lab_CenFreq =       new QLabel("中频频率[KHz]:");
    QLabel* lab_DataOffset =    new QLabel("读取偏移Bytes:");
    QLineEdit* lEd_SampRate =   new QLineEdit();
    QLineEdit* lEd_TunFreq =    new QLineEdit();
    QLineEdit* lEd_CenFreq =    new QLineEdit();
    QLineEdit* lEd_DataOffset = new QLineEdit();
signals:
    //信息路由从，一个模块到另一个模块 ucCmdContent:数据内容  nDataLength：数据长度  uusInfoType：信息类别号 usDestID：目标地址
    void ModuleDataRouter(char* cCmdContent, int nDataLength, unsigned short usInfoType, unsigned short usDestID);
    void SendFileMsg(Data_File_Attribute stSendData);
    void Sendok();
public slots:
    void Finish(Data_File_Attribute stRecvFileData)
    {
        qDebug() << G_stDataSave.uint16_Data[0] << G_stDataSave.uint16_Data[1] << G_stDataSave.uint16_Data[2] << G_stDataSave.uint16_Data[3];
        emit Sendok();
        ModuleDataRouter((char*)&stRecvFileData, sizeof(Data_File_Attribute), INNER_PARAM_UIDSP, MOUDLE_MAINFRAME);
    }
};


/*使用线程池的方法创建线程任务，使用时按需进行调度*/
//读数据
class DealData:public QObject, public QRunnable
{
    Q_OBJECT
    void run() override     // override关键字用于重载一个虚函数
    {
        //读取文件数据
        QFile _file(stRecvFileData._fileName);
        if(!_file.open(QIODevice::ReadOnly))       //只读文件
        {
            QMessageBox::warning(0, "Warning", "打开文件时出错!");
            return;
        }
        QDataStream _data(&_file);
        //_data.setByteOrder(QDataStream::BigEndian);  //以小端模式读取，先读取低字节（避免了再对读取出来的数据进行大小端转换
        m = new char[stRecvFileData.lDataSzie];
        _data.readRawData(m, stRecvFileData.lDataSzie);   // readRawData  读取原始的二进制格式
        ReadData();
        delete[] m;
        _file.close();
    }
public:
    Data_File_Attribute stRecvFileData;
    char* m;        //文件数据源数据
    void ReadData() //根据不同数据类型读取文件
    {
        switch(G_stDataParam.cDataType)  // 全局UI参数保存  DataParam主界面数据参数汇总
        {
        case 0: //uint8_t
        {
            char* res = new char[stRecvFileData.lDataSzie / sizeof(char)];
            memset(res, 0, stRecvFileData.lDataSzie / sizeof(char));
            //void *memset(void *s, int c, unsigned long n);
            //函数的功能是：将指针变量 s 所指向的前 n 字节的内存单元用一个“整数” c 替换
            memcpy(res, m, stRecvFileData.lDataSzie);
            //void *memcpy(void*dest, const void *src, size_t n);
            //由src指向地址为起始地址的连续n个字节的数据复制到以destin指向地址为起始地址的空间内
            G_stDataSave.uint8_Data = res;
            G_stDataSave.nDataLen = stRecvFileData.lDataSzie / sizeof(char);
        }break; //uint16_t
        case 1:
        {
            short* res = new short[stRecvFileData.lDataSzie / sizeof(short)];
            memset(res, 0, stRecvFileData.lDataSzie / sizeof(short));
            memcpy(res, m, stRecvFileData.lDataSzie);
            G_stDataSave.uint16_Data = res;
            G_stDataSave.nDataLen = stRecvFileData.lDataSzie / sizeof(short);
        }break;
        case 2: //uint32_t
        {
            int* res = new int[stRecvFileData.lDataSzie / sizeof(int)];
            memset(res, 0, stRecvFileData.lDataSzie / sizeof(int));
            memcpy(res, m, stRecvFileData.lDataSzie);
            G_stDataSave.uint32_Data = res;
            G_stDataSave.nDataLen = stRecvFileData.lDataSzie / sizeof(int);
        }break;
        default:break;
        }
        //添加处理
        //int nFFTSize = 128*qPow(2, G_stDataParam.cFFTOperNum);  //FFT点数
        //int nSize = G_stDataSave.nDataLen / nFFTSize;           //做多少次FFT
        G_stDataSave.dData = new double[G_stDataSave.nDataLen];
        G_stDataSave.lData = new long[G_stDataSave.nDataLen];
        //    double* dData;   long*   lData;
        memset(G_stDataSave.dData, 0, sizeof(double)*G_stDataSave.nDataLen);
        for(unsigned int i = 0; i < G_stDataSave.nDataLen; i++)
        {
            if(0 == G_stDataParam.cDataType)
            {
                G_stDataSave.dData[i] = G_stDataSave.uint8_Data[i];
                G_stDataSave.lData[i] = G_stDataSave.uint8_Data[i];
            }
            else if(1 == G_stDataParam.cDataType)
            {
                G_stDataSave.dData[i] = G_stDataSave.uint16_Data[i];
                G_stDataSave.lData[i] = G_stDataSave.uint16_Data[i];
            }
            else if(2 == G_stDataParam.cDataType)
            {
                G_stDataSave.lData[i] = G_stDataSave.uint32_Data[i];
            }
        }
        emit Finished(stRecvFileData);
    }
signals:
    void Finished(Data_File_Attribute stRecvFileData);
public slots:
    void RecvFileMsg(Data_File_Attribute stRecvData)
    {
        stRecvFileData = stRecvData;
    }
};
#endif // FILECONTROL_H
