#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include <QImage>
#include <QThread>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    myt = new MyThread;
    thread = new QThread(this);
    myt->moveToThread(thread);
    thread->start();
    connect(ui->pushButton, &QPushButton::clicked, myt , &MyThread::drawImage);
    connect(myt, &MyThread::updateImage, this, &Widget::updata);
    connect(this,&Widget::destroyed, this ,&Widget::close);
}

void Widget::close()
{
    thread->quit();
    thread->wait();
    delete myt;
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(30,30,image);
}

void Widget::updata(QImage temp)
{
    image = temp;
    update();
}

Widget::~Widget()
{
    delete ui;
}

