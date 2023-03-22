#ifndef MYTHREAD_H
#define MYTHREAD_H
#pragma execution_character_set("utf-8")
#include <QThread>
#include <QVector>
#include <QRunnable>
// 生成随机数
class Generate : public QObject ,public QRunnable
{
    Q_OBJECT
public:
    explicit Generate(QObject *parent = nullptr);

    void run();
signals:
    void sendArray(QVector<int> num);
private:
    int my_num;
public slots:
    void recv_num(int n);
};


// 冒泡排序
class BubbleSort : public QObject ,public QRunnable
{
    Q_OBJECT
public:
    explicit BubbleSort(QObject *parent = nullptr);

    void run();
signals:
    void finishArray(QVector<int> list);
private:
    QVector<int> my_list;
public slots:
    void recv_list(QVector<int> list);
};




#endif // MYTHREAD_H
