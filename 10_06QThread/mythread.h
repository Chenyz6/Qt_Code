#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
protected:
    //QThread的虚函数
    //线程处理函数 不能直接调用 需要使用start间接调用
    void run();
signals:
    void isDone();
};

#endif // MYTHREAD_H
