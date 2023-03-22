#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#pragma execution_character_set("utf-8")
class MyThread : public QObject
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
    void timeout();
    void setflag(bool flag = true);
signals:
    void MySignal();
private:
    bool isStop;
};

#endif // MYTHREAD_H
