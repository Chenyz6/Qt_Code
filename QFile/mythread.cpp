#include "mythread.h"
QVector<double> xTemp;
QVector<double> yTemp;
MyThread::MyThread(QObject *parent) : QThread(parent)
{

}

void MyThread::run()
{
    //很复杂的数据处理
//    double* y = new double[G_stDataSave.nDataLen / 2];
//    y = CalSingalFrequency(G_stDataSave.dData,long(G_stDataSave.nDataLen));
//    for(int i = 0 ; i < int(G_stDataSave.nDataLen / 2) ;i++)
//    {
//        xTemp << i;
//        yTemp << y[i];
//    };

    emit isDone();
}

double* MyThread::CalSingalFrequency(double* pData, long nDataLen) //获取信号瞬时频率
{
    double* pResult = new double[nDataLen / 2];
    double ti, tr;
    for (long i = 0; i < nDataLen / 2 - 1; i++)
    {
        ti = pData[2 * i] * pData[2 * i + 3] - pData[2 * i + 2] * pData[2 * i + 1];
        tr = pData[2 * i + 2] * pData[2 * i] + pData[2 * i + 3] * pData[2 * i + 1];

        pResult[i] = atan2(ti, tr);
    }
    pResult[nDataLen / 2 - 1] = 0.0;
    return pResult;
}


