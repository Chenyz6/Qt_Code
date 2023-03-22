#include "mywidget.h"
#include "ui_mywidget.h"
#include<QPushButton>
#include"mybutton.h"
MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)  //给基类参数列表传参
    , ui(new Ui::MyWidget)
{
    move(300,300);  // 相对于屏幕左上角 包括边框

    QPushButton * b1 = new QPushButton(this);
    b1->move(100,100);   // 不包括边框
    b1->setText("^_^");
    b1->resize(100,100);


    QPushButton * b2 = new QPushButton(b1);
    b2->move(10,10);  // 相对于父窗口
    b2->setText("@_@");

    MyButton *b3 = new MyButton(this);
    b3->setText("123");
    //1.指定父对象后  2.直接或间接继承于QOBject
    //子对象如果是动态分配空间的，不需要手动释放delete，系统会自动释放
    ui->setupUi(this);
}

MyWidget::~MyWidget()
{
    delete ui;
}

