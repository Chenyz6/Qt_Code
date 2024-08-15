#include "widget.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget mainWidget;
    mainWidget.resize(960,640);
    mainWidget.setWindowTitle(QString::fromUtf8("Qt/C++架构工程demo版本V1.0.0"));
    mainWidget.setWindowIcon(QIcon(":/resource/main_ico.ico"));
    mainWidget.setStyleSheet("font: bold 12px;");
    mainWidget.show();

    return a.exec();
}
