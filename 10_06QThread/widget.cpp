#include "widget.h"
#include "ui_widget.h"
#include <QThread>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    mytimer = new QTimer(this);
    //定时器启动 自动触发timeout
    connect(mytimer, &QTimer::timeout, this, &Widget::lcdfunction);
    thread = new MyThread(this);
    connect(thread, &MyThread::isDone, this, &Widget::DealDone);
    //当点击右上角关闭的时候，关闭线程
    connect(this, &Widget::destroy, this, &Widget::stopThread);
}

void Widget::stopThread()
{
    //停止线程
    thread->quit();
    //等待线程处理完手头工作
    thread->wait();
}

void Widget::DealDone()
{
    qDebug() << "over" ;
    mytimer->stop();

}

void Widget::lcdfunction()
{
    static int i = 0;
    i++;
    ui->lcdNumber->display(i);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    if(mytimer->isActive() == false)
    {
        mytimer->start(100);
    }

    //启动线程
    thread->start();

}
