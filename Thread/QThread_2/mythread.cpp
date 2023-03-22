#include "mythread.h"
#include <QElapsedTimer>
#include <QDebug>
#include <QThread>
Generate::Generate(QObject *parent) : QObject(parent)
{

}

void Generate::working(int num)
{
    qDebug() << "生成数据线程地址：" << QThread::currentThread();
    QVector<int> list;
    QElapsedTimer time;
    time.start();
    for(int i = 0; i < num; i++)
    {
        list << qrand() % 100000;
    }
    time.elapsed();
    qDebug() << "生成" << num << "个时间用时：" << time.elapsed() << "ms";
    emit sendArray(list);
}



BubbleSort::BubbleSort(QObject *parent) : QObject(parent)
{

}

void BubbleSort::working(QVector<int> list)
{
    qDebug() << "冒泡排序线程地址：" << QThread::currentThread();
    QElapsedTimer time;
    time.start();
    for(int i = 0; i < list.size() - 1;i++)
    {
        for(int j = 0; j < list.size() - 1 - i; j++)
        {
            if(list[j] > list[j + 1])
            {
                int temp;
                temp = list[j + 1];
                list[j + 1] = list[j];
                list[j] = temp;
            }
        }
    }
    time.elapsed();
    qDebug() << "冒泡排序时间用时：" << time.elapsed() << "ms";
    emit finishArray(list);
}







