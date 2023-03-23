#include "serverwidget.h"

#include <QApplication>
#include "clientwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerWidget w;
    w.show();
    ClientWidget c;
    c.show();
    return a.exec();
}
