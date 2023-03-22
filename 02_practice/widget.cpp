#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include<QDebug>   //打印


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    setWindowTitle("老大");
    b1.setParent(this);
    b1.setText("close");
    b1.move(100, 100);

    b2 = new QPushButton(this);
    b2->setText("abc");

    connect(&b1, &QPushButton::pressed, this, &Widget::close);
    /*  &b1:信号发出者，指针类型
        &QPushButton::pressed 处理的信号  &发送者的类名::信号名字
        this：信号的接受者
        &Widget::close 槽函数，信号处理函数  &接受的类名::槽函数
    */

    //自定义槽 普通函数的用法
    /*槽函数需要和信号一致（参数，返回值）
      由于信号都是没有返回值，所以，槽函数一定没有返回值
    */
    //    clicked：鼠标点击按钮一次，信号就会被触发
    //    pressed：鼠标的左键 | 右键按下，信号就会被触发
    //    released：按钮被释放，信号就会被触发

    connect(b2, &QPushButton::released, this , &Widget::mySlot);
    connect(b2, &QPushButton::released, &b1 , &QPushButton::hide);



    this->setWindowTitle("老大");
    b3.setParent(this);
    b3.setText("切换到子窗口");
    b3.move(50,50);

    connect(&b3, &QPushButton::released, this, &Widget::changeWin);
    //处理子窗口的信号
    void (SubWidget::*Signal1)() = &SubWidget::mySignal;
    connect(&zi, Signal1, this, &Widget::dealSub);
    void (SubWidget::*Signal2)(int, QString) = &SubWidget::mySignal;
    connect(&zi,Signal2,this,&Widget::dealSlot);

    //QT4信号连接
    //QT4槽函数 必须要有slots关键字来修饰
//    connect(&zi, SIGNAL(mySignal()),this,SLOT(dealSub()));
//    connect(&zi, SIGNAL(mySignal(int ,QString)),
//            this,SLOT(dealSlot()));


    //Lambda表达式  匿名函数对象
    //Lambda表达式用于定义并创建匿名的函数对象，巧用Lambda表达式可以简化编程工作
    // C++ 增加的新特性，项目文件  CONFIG += C++11
    //QT配合信号一起使用 十分方便
    QPushButton *b4 = new QPushButton(this);
    b4->setText("Lambda表达式");
    b4->move(150,150);
    int a = 10, b = 100;
    connect(b4,&QPushButton::clicked,
            // = 把外部所有局部变量，类中所有成员以值方式传递
            // this:类中所有成员以值方式传递
            // & 把外部所有局部变量  引用符号
            [=](bool isCheck) mutable  //值可以变
            //不要用引用 因为会占着内存 最好用=
            {
                b4->setText("123");
                qDebug() << "111111";
                qDebug() << a << b;
                a = 5;
                qDebug() << a << b;
                qDebug() << isCheck;  // false
            }
            );

    resize(400,300);
}

void Widget::dealSlot(int a, QString str)
{
    qDebug() << a << str.toUtf8().data() ;
    // str.toUtf8()  -> 字节数组QByteArray
}

void Widget::changeWin()
{
    //子窗口显示 本窗口隐藏
    zi.show();
    this->hide();
}

void Widget::dealSub()
{
    zi.hide();
    this->show();
}

void Widget::mySlot()
{
    b2->setText("123");
}

Widget::~Widget()
{
    delete ui;
}

