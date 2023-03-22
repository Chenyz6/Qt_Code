#include "widget.h"
#include "ui_widget.h"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //stylesheet  样式表
    this->setStyleSheet("QLabel{color:rgb(0,200,255);"
                             "background-color:red;"
                        "border-image:url(:/image/1.jpg) 10 10 10 10 stretch stretch;}");
    ui->label->setStyleSheet("QLabel{color:rgb(0,200,255);"

                             "}");


}

Widget::~Widget()
{
    delete ui;
}

