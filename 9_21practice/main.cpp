#include "widget.h"

#include <QApplication>
#include <QPushButton>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    QPushButton b(&w);
    b.setText("123456");
    b.move(100,100);
    b.show();


    return a.exec();
}
