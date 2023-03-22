#include "widget.h"
#include "ui_widget.h"
#include <QThread>
#include <QDebug>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
//    面试题
//    connect（）  第五个参数的作用？连接方式（默认，队列，直接） 多线程时才有意义
//    如果是多线程，默认使用队列
//    如果是单线程，默认使用直接方式
//    队列：槽函数所在的线程和接收者一样
//    直接：槽函数所在线程和发送者一样

    ui->setupUi(this);
    //动态分配空间 不能指定父对象
    Mytimer = new MyThread;  //如果制定了父对象  后面就没有办法move了
    //创建子线程
    thread = new QThread(this);
    //把自定义线程加入到子线程中
    Mytimer->moveToThread(thread);
    thread->start();
    connect(Mytimer, &MyThread::MySignal , this, &Widget::dealsignal);
    qDebug() << "主线程号" << QThread::currentThread();

    connect(this,&Widget::startthread,Mytimer,&MyThread::timeout);
    connect(this, &Widget::destroyed, this, &Widget::dealdestroy);

    //线程处理函数内部  不允许操作图形界面
}

void Widget::dealdestroy()
{
    on_pushButtonStop_clicked();
    delete Mytimer; //注意 没有指定父对象  要删delete
}

void Widget::dealsignal()
{
    static int i = 0;
    i++;
    ui->lcdNumber->display(i);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButtonStart_clicked()
{
    if(thread->isRunning() == true)
    {
        return;
    }
    //启动线程 但是没有启动线程处理函数
    thread->start();
    Mytimer->setflag(false);
    //不能直接调用线程处理函数
    //直接调用的话就会导致线程处理函数和主线程在同一个线程
    //只能通过信号和槽的方式
    emit startthread();

}

void Widget::on_pushButtonStop_clicked()
{
    if(thread->isRunning() == false)
    {
        return;
    }
    Mytimer->setflag(true);
    thread->quit(); //先把手头的工作处理完 但是因为手头工作是一个while（1） 所以没有办法处理完
    thread->wait(); //wait方法会将当前线程放入wait set，等待被唤醒
}
