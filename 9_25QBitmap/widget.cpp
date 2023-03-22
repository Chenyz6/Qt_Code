#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QBitmap>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(0,0 ,200,200, QPixmap("../image/2.png"));
    p.drawPixmap(0,200 ,200,200, QBitmap("../image/2.png"));
    //另一种显示图片方法
    QPixmap pixmap;
    pixmap.load("../image/2.png");
    p.drawPixmap(0,400 ,200,200, pixmap);
}

Widget::~Widget()
{
    delete ui;
}

