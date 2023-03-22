#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QImage>
class MyThread : public QObject
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
    void drawImage();//线程处理函数
signals:
    void updateImage(QImage temp);
};

#endif // MYTHREAD_H
