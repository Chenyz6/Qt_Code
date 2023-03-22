#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen pen;
    pen.setWidth(5);
    pen.setStyle(Qt::CustomDashLine);
    pen.setColor(QColor(0,255,255));
    p.setPen(pen);//设置画笔
    QBrush brush;
    brush.setColor(Qt::red);
    brush.setStyle(Qt::VerPattern);
    p.setBrush(brush);//设置笔刷
    p.drawRect(0,0,30,30);
    p.drawEllipse(40,40,30,10);

    p.drawLine(50,50,150,150);
    p.drawLine(50,50,300,200);
}



Widget::~Widget()
{
    delete ui;
}

