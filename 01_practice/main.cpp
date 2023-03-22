#include "widget.h"
#include <QPushButton>

//QApplication 应用程序类
//QT头文件没有.h
//头文件和类名一样
#include <QApplication>

int main(int argc, char *argv[])
{
    //有且只有一个应用程序类的对象
    QApplication a(argc, argv);

    //Widget继承于QWidget--窗口基类
    //w 是一个窗口
    Widget w;  // widget 部件
    w.setWindowTitle("你好");  //  设置标题


    QPushButton b;
    b.setParent(&w);   //第一种方式
    b.setText("^_^");
    b.move(100,100);
    b.show();
    //不指定父对象 窗口是独立的

    QPushButton b1(&w);
    b1.setText("abc");   //第二种方式
    b1.show();

    w.show();
    return a.exec();  // 让程序一直执行 等待用户操作 等待事件发生
}
