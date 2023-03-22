#include "mythread.h"
#include <QElapsedTimer>
#include <QDebug>
Generate::Generate(QObject *parent) : QObject(parent) , QRunnable()
{
    setAutoDelete(true);
}

void Generate::run()
{
    qDebug() << "生成数据线程地址：" << QThread::currentThread();
    QVector<int> list;
    QElapsedTimer time;
    time.start();
    for(int i = 0; i < my_num; i++)
    {
        list << qrand() % 100000;
    }
    time.elapsed();
    qDebug() << "生成" << my_num << "个时间用时：" << time.elapsed() << "ms";
    emit sendArray(list);
}

void Generate::recv_num(int n)
{
    my_num = n;
}

BubbleSort::BubbleSort(QObject *parent) : QObject(parent) , QRunnable()
{
    setAutoDelete(true);
}

void BubbleSort::run()
{
    qDebug() << "冒泡排序线程地址：" << QThread::currentThread();
    QElapsedTimer time;
    time.start();
    for(int i = 0; i < my_list.size() - 1;i++)
    {
        for(int j = 0; j < my_list.size() - 1 - i; j++)
        {
            if(my_list[j] > my_list[j + 1])
            {
                int temp;
                temp = my_list[j + 1];
                my_list[j + 1] = my_list[j];
                my_list[j] = temp;
            }
        }
    }
    time.elapsed();
    qDebug() << "冒泡排序时间用时：" << time.elapsed() << "ms";
    emit finishArray(my_list);
}

void BubbleSort::recv_list(QVector<int> list)
{
    my_list = list;
}







