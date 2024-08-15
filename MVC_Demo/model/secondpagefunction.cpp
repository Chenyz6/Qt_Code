#include "secondpagefunction.h"
#include <QDebug>

QSecondPageFunction::QSecondPageFunction(QObject *parent) : QObject(parent)
{
}

void QSecondPageFunction::count_slot()
{
    int a = 2;
    int b = 3;
    qDebug()<<tr("页面2的逻辑功能计算结果为==%1").arg(a*b);
}
