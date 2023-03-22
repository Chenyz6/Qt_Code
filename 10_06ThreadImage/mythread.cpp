#include "mythread.h"
#include <QThread>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QImage>
MyThread::MyThread(QObject *parent) : QObject(parent)
{

}

void MyThread::drawImage()
{

    QImage image(500,500,QImage::Format_ARGB32);
    QPainter p(&image);
    QPoint a[] =
    {
        QPoint(qrand()%500,qrand()%500),
        QPoint(qrand()%500,qrand()%500),
        QPoint(qrand()%500,qrand()%500),
        QPoint(qrand()%500,qrand()%500),
        QPoint(qrand()%500,qrand()%500)
    };
    QPen pen;
    pen.setWidth(5);
    pen.setColor(Qt::red);
    p.setPen(pen);
    QBrush brush;
    brush.setColor(Qt::red);
    brush.setStyle(Qt::Dense1Pattern);
    p.setBrush(brush);
    p.drawPolygon(a,5);
    emit updateImage(image);
}




