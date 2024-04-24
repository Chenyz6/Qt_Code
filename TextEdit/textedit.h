#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QWidget>
#include <QThread>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include "global.h"

#pragma execution_character_set("utf-8")

// 信道编码算法头文件
#include "./ChannelCoding/rc_bch.h"
#include "./ChannelCoding/rc_rs.h"
#include "./ChannelCoding/rc_dict.h"
#include "./ChannelCoding/DataRead_Write.h"
#include "./ChannelCoding/clsm_viterbi.h"
#include "./ChannelCoding/conv_lsm_codec.h"
#include <cstdlib>
#include <cstdio>
#include <memory.h>
#include <vector>
#include "./ChannelCoding/AwgnChannel.h"
#include "./ChannelCoding/SignalGenerator.h"
#include "./ChannelCoding/LdpcDecode.h"
#include "./ChannelCoding/BpskModulate.h"
#include "./ChannelCoding/LdpcEncode.h"
#include "./ChannelCoding/Matrix.h"
using namespace RC_DICTCODEC;
using namespace LSMVIT;

namespace Ui {
class TextEdit;
}

// 信道编码线程
class ChannelCoding : public QThread
{
    Q_OBJECT
public:
    ChannelCoding(QWidget *parent = nullptr);
    ~ChannelCoding();
signals:
    void IdentifyResult(int result);  // 编码识别结果
protected:
    void run();
public slots:
    void ReceiveIndex(short index);     // 接受信道编码第几个模式
    void ReceiveChannelCodingPara(char * cCmdContent, int nDataLength); // 接受编码参数
private:
    short index;    // 信道编码第几个模式
    ChannelCodingPara channelCodingPara; // 编码参数
    long fileSize;      // 文件读取长度
    const int PrimePloy[69] = { 13,23,45,67,75,103,147,155,203,211,217,235,277,31,325,345,367,
    435,453,537,543,545,551,703,747,1021,1055,1131,1157,1167,1175,2011,2033,2157,
    2443,2745,3471,4005,4445,5023,5263,6211,7363,10123,11417,12515,13505,14127,15053,
    20033,23261,24633,30741,32535,37505,42103,51761,55753,60153,71147,67401,100003,
    110013,120265,133663,142305 };
    const int PrimePloySize = sizeof(PrimePloy) / sizeof(PrimePloy[0]);
public:
    void demo_bch(int n, int k, int Gnum, size_t *pins, char *cpData,int DataLen, char *result);
    void demo_rs(int m, char *g, int n, int k, char *cpData, int DataLen, char *result);
    void demo_conv(int n, int k, int m, int *pins, char *cpData, int DataLen, char *result);
    void demo_dessr(int *g, int gSize, char* cpData, int DataLen, char* result);
    void demo_tpc(int n, int k, int Gnum, size_t *pins, char *cpData,int DataLen, char *result);
    char* FrameLengthDetect(char* cpData, DWORD DataLen, int m, int nb, int *ResultDataLen);
    vector<int> demo_ldpc(int n, int k, char* cpData, DWORD DataLen, vector<int> result);
    // 编码识别
    int gcd(int a, int b);
    int RC_ModeAnalysis(int compare);
    int BCH_n_detect(char* cpData, int DataLen);
    int LDPCdetect(char* cpData, int DataLen);
    int demo_dessr(vector<int> g, int gSize, char* cpData, int DataLen, char* result);
    double average(double *x, int len);
    double variance(double *x, int len);
    double t_test(double *x, double *y, int len_x, int len_y);
    int desser(char* cpData, int DataLen);
};

// 文件处理线程
class FileReadThread : public QThread{
    Q_OBJECT
public:
    FileReadThread(QWidget *parent = nullptr);
    ~FileReadThread();
protected:
    void run();
signals:
     void SendData(QString buf);           // 发送处理完的数据
};


class TextEdit : public QWidget
{
    Q_OBJECT
public:
    explicit TextEdit(QWidget *parent = nullptr);
    ~TextEdit();

    void OpenFrameStructureDetection(ChannelCodingPara * channelCodingPara); // 启动帧结构检测
    void OpenBCH(ChannelCodingPara * channelCodingPara);               // 启动BCH编码
    void OpenRS(ChannelCodingPara * channelCodingPara);                // 启动RS编码
    void OpenCONV(ChannelCodingPara * channelCodingPara);              // 启动CONV编码
    void OpenDESSR(ChannelCodingPara * channelCodingPara);             // 启动DESSR编码
    void OpenTPC(ChannelCodingPara * channelCodingPara);               // 启动TPC
    void OpenLdpc(ChannelCodingPara * channelCodingPara, int ldpc_n, int ldpc_k);  // 启动LDPC
    void OpenCodingIdentify();                                         // 启动编码识别
    void OpenFile();                                                   // 打开文件
    void InitPlainTextEdit();          // 初始化文本编辑器
    void InitTextEdit();               // 初始化行号
    void InitSignalSlot();             // 初始化信号和槽
public slots:
    void PlainTextEditMousePress(QMouseEvent* event);		// 获取鼠标点击位置
    void PlainTextEditMouseMove(QMouseEvent* event);		// 获取鼠标移动位置
    void PlainTextEditMouseRelease(QMouseEvent* event);		// 获取鼠标松开的位置
    void PlainTextEditWheel(QWheelEvent* event);			// 获取鼠标滚轮事件
signals:
    void SendCodingIndex(short index);           // 发送信道编码模式标志位
    void SendChannelCodingPara(char * cCmdContent, int nDataLength); // 发送编码参数
    void SendThreadFinish(short index);            // 发送线程完毕标志位
    void filePath();                               // 发送文件路径

private:
    Ui::TextEdit *ui;
    ChannelCoding * channelCoding;          // 信道编码线程对象
    FileReadThread * fileReadThread;        // 文件处理线程
    unsigned long long lineNumber = 0;          // 行号
};

#endif // TEXTEDIT_H
