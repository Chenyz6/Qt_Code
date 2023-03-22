#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QMouseEvent>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //去窗口边框
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    //把窗口背景设为透明
    setAttribute(Qt::WA_TranslucentBackground);
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(0,0,QPixmap("../image/5.png"));
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        close();
    }
    else if(e->button() == Qt::LeftButton)
    {
        p = e->globalPos() - this->frameGeometry().topLeft();
    //globalPos()  相对于屏幕左上角
    //frameGeometry().topLeft()  包括边框的左上角坐标
    }
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() == Qt::LeftButton)
    {
        move(e->globalPos() - p);
    }
}

Widget::~Widget()
{
    delete ui;
}

