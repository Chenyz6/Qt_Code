#include "firstpagefunction.h"
#include "controller/firstpagecontroller.h"
#include <QDebug>

QFirstPageFunction::QFirstPageFunction(QObject *parent) : QObject(parent)
{    
}

void QFirstPageFunction::count_slot()
{
    int a = 2;
    int b = 3;
    QFirstPageController::GetInstance()->finishedCount_signal();
    qDebug()<<tr("页面1的逻辑功能计算结果为==%1").arg(a+b);
}
