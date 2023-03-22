#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QPixmap pixmap(500,500);

    QPainter p(&pixmap);
    //填充背景色 第一种方法
    //pixmap.fill(Qt::black);
    //第二种方法
    p.fillRect(0,0,500,500,QBrush(Qt::black));

    p.drawPixmap(0,0,200,200,QPixmap("../image/2.png"));
    pixmap.save("../9_25QPixmap/pixmap.png");
}

Widget::~Widget()
{
    delete ui;
}

