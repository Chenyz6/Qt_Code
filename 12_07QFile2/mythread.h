#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include "Filecontrol.h"

extern QVector<double> xTemp;
extern QVector<double> yTemp;
class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
    double* CalSingalFrequency(double* pData, long nDataLen);
protected:
    //QThread的虚函数
    //线程处理函数 不能直接调用 需要使用start间接调用
    void run();
signals:
    void isDone();
};

#endif // MYTHREAD_H
