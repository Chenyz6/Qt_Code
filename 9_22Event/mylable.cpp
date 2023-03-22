#include "mylable.h"
#include <QMouseEvent>
#include <QDebug>
MyLable::MyLable(QWidget *parent) : QLabel(parent)
{
    this->setMouseTracking(true);  // 设置默认的鼠标
}

void MyLable::mousePressEvent(QMouseEvent *ev)
{
    int i = ev->x();
    int j = ev->y();
    /*
     * QString str = QString("abc %1 ^_^ %2").arg(123).arg("shihf")
     *
    */
    if(ev->button() == Qt::LeftButton)
    {
       qDebug() << "left";
    }else if(ev->button() == Qt::RightButton)
    {
        qDebug() << "right";
    }else if(ev->button() == Qt:: MidButton)
    {
        qDebug() << "mid";
    }

    QString text = QString("<center><h1>Mouse Press:(%1, %2)</h1></center>")
            .arg(i).arg(j);
    this->setText(text);
;
}

void MyLable::mouseReleaseEvent(QMouseEvent *ev)
{
    QString text = QString("<center><h1>Mouse Release:(%1, %2)</h1></center>")
            .arg(ev->x()).arg(ev->y());
    this->setText(text);
}

void MyLable::mouseMoveEvent(QMouseEvent *ev)
{
    QString text = QString("<center><h1>Mouse Move:(%1, %2)</h1></center>")
            .arg(ev->x()).arg(ev->y());
    //this->setText(text);
}

void MyLable::enterEvent(QEvent *e)
{
    QString text = QString("<center><h1>Mouse Enter</h1></center>");
    this->setText(text);
}

void MyLable::leaveEvent(QEvent *e)
{
    QString text = QString("<center><h1>Mouse Leave</h1></center>");
    this->setText(text);
}



