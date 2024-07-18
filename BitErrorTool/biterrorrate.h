#ifndef BITERRORRATE_H
#define BITERRORRATE_H

#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include <QtCharts>
#include <iostream>
using namespace std;

namespace Ui {
class BitErrorRate;
}

typedef struct {
    unsigned long bit_num;       /* 统计的码元个数 */
    unsigned long bit_error_num; /* 错误的码元个数 */
    double ber;                  /* 误码率 */
    unsigned long bit_length;    /* 比特序列长度*/
} ber_test_result;

class BitErrorRateThread : public QThread
{
    Q_OBJECT
protected:
    void run();
public:
    BitErrorRateThread();
    ~BitErrorRateThread();

    bool bit_error_rate(ber_test_result *result, unsigned long start_postion,
                        string transmit_seq, string demod_seq);
signals:
    void EmitBitNumResult(char* result, int length); // 发送误码率结果
    void ProgressValue(double value);   // 进度条比例
public slots:
    void RecBitErrorPara(QString fileName, QString bitFileName,
                         unsigned int startOffset); // 接收误码率参数
private:
    string fileName;       // 测试文件路径
    string bitFileName;    // 比特序列文件路径
    unsigned int startOffset;   // 起始位置
    unsigned long bit_len;  // 比特序列长度
};

class BitErrorRate : public QWidget
{
    Q_OBJECT
signals:
    void EmitBitErrorPara(QString fileName, QString bitFileName,
                          unsigned int startOffset);    // 发送误码率参数

public:
    explicit BitErrorRate(QWidget *parent = nullptr);
    ~BitErrorRate();

    void InitPieSeries();    // 初始化饼图
    void ShowPieSeriesResult(unsigned int bit_error, unsigned int bit_right); // 显示结果在饼图上
    void ReadIniFile();     // 读取配置文件
    void WriteIniFile();    // 写入配置文件

private slots:
    void on_pushButton_filefile_clicked();      // 测试文件路径
    void on_pushButton_bitfilepath_clicked();   // 比特序列文件路径
    void on_pushButton_start_clicked();         // 开始测试
    void onPieSeriesClicked(QPieSlice* slice);  // 点击饼图
private:
    Ui::BitErrorRate *ui;

    QString fileName;       // 测试文件路径加名字
    QString bitFileName;    // 比特序列文件路径加名字

    QString fileNamePath;       // 测试文件路径
    QString bitFileNamePath;    // 比特序列文件路径

    QPieSeries *pie_series; // 饼状图
    BitErrorRateThread * bitErrorRateThread = new BitErrorRateThread; // 误码率线程对象
};

#endif // BITERRORRATE_H
