#include "serverwidget.h"

#include <QApplication>
#include "clientwidget.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    serverwidget w;
    w.show();
    clientwidget c;
    c.show();
    return a.exec();
}
