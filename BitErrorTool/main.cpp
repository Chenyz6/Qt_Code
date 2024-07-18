#include "biterrorrate.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BitErrorRate w;
    w.show();
    return a.exec();
}
