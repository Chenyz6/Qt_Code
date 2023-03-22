#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QImage>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    myt = new MyThread;
    thread = new QThread(this);
    myt->moveToThread(thread);
    thread->start();
    connect(ui->pushButton,&QPushButton::clicked,myt,&MyThread::drawImage);
    connect(myt,&MyThread::sendImage,this,&Widget::updateimage);
}

void Widget::updateimage(QImage temp)
{
    image = temp;
    update();
}

void Widget::paintEvent(QPaintEvent* )
{
    QPainter p(this);
    p.drawImage(30,30,image);
}

Widget::~Widget()
{
    delete ui;
}

