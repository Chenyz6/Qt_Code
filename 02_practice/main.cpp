#include "widget.h"
#include <QPushButton>
#include <QApplication>

int main(int argc, char *argv[])
{
    //有且只有一个应用程序类的对象
    QApplication a(argc, argv);

    //Widget继承于QWidget--窗口基类
    //w 是一个窗口
    Widget w;  // 执行Widget的构造函数
    w.show();

    return a.exec();  // 让程序一直执行 等待用户操作 等待事件发生
}
