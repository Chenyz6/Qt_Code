#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QImage image(800,800,QImage::Format_ARGB32);//QImage::Format_ARGB32 背景透明
    QPainter p(&image);
    p.drawImage(0,0,QImage("../image/2.png"));
    //pixel--像素
    for(int i = 0;i < 50;i++)
    {
        for(int j = 0; j <100; j ++)
        {
            image.setPixel(i,j,qRgb(0,255,0));
        }
    }
    image.save("../9_25QImage/image.png");

}

Widget::~Widget()
{
    delete ui;
}

