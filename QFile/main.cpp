#include "mainwindow.h"

#include <QApplication>
#include <Common_Inner.h>

int main(int argc, char *argv[])
{
    //注册自定义信号槽
    RegisterMetaType CRegisterMetaType;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
