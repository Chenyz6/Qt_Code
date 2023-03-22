#include "widget.h"
#include "ui_widget.h"
#include <addwidget.h>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    resize(500,500);
    setWindowTitle("测试例子");
    c.setText("cyz");
    c.setParent(this);
    //connect(&c, &QPushButton::clicked, this, &Widget::convert);
    c.show();
    d.setParent(this);
    d.move(200,200);
    connect(&c, &QPushButton::clicked, this, &Widget::convert);
    connect(&c, &QPushButton::released, this, &Widget::convert1);
}

void Widget::convert()
{
    zi.show();
    this->hide();
}

void Widget::convert1()
{
    zi.hide();
    this->show();
}


Widget::~Widget()
{
    delete ui;
}

