#include "mythread.h"
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
    QPoint a[] = {
        QPoint(qrand()%500,qrand()%500),
         QPoint(qrand()%500,qrand()%500),
         QPoint(qrand()%500,qrand()%500),
         QPoint(qrand()%500,qrand()%500),
         QPoint(qrand()%500,qrand()%500)
    };
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(2);
    p.setPen(pen);
    QBrush brush;
    brush.setColor(Qt::red);
    brush.setStyle(Qt::Dense3Pattern);
    p.setBrush(brush);
    p.drawPolygon(a,5);

    emit sendImage(image);
}
