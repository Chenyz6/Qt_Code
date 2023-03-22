#include "mythread.h"
#include <QThread>
#include <QDebug>
MyThread::MyThread(QObject *parent) : QObject(parent)
{
    isStop = false;
}

void MyThread::timeout()
{
    while (false == isStop)
    {
        QThread::sleep(1);
        emit MySignal();
        qDebug() << "子线程号" << QThread::currentThread();

        if(true == isStop)
        {
            break;
        }
    }
}

void MyThread::setflag(bool flag)
{
    isStop = flag;
}
