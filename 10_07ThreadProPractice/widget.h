#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "mythread.h"
#include <QImage>
#include <QThread>
#include <QImage>
#pragma execution_character_set("utf-8")
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void paintEvent(QPaintEvent*);
    void updateimage(QImage);


private:
    Ui::Widget *ui;
    QImage image;
    MyThread *myt;
    QThread *thread;
};
#endif // WIDGET_H
