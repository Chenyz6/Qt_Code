#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QPicture>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QPicture picture;
    QPainter p;
    p.begin(&picture);
    p.drawPixmap(0,0,500,500,QPixmap("../image/3.png"));
    p.drawLine(50,50,200,50);

    picture.save("../image/9_25QPicture/picture.png");
    p.end();
}

void Widget::paintEvent(QPaintEvent *)
{
    QPicture p;
    p.load("../image/9_25QPicture/picture.png");
    QPainter e(this);
    e.drawPicture(0,0,p);



}

Widget::~Widget()
{
    delete ui;
}

