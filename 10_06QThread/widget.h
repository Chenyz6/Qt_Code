#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>   //定时器头文件
#include <QDebug>
#include <mythread.h>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void lcdfunction();//定时器槽函数
    void DealDone();//线程结束槽函数
    void stopThread();//停止线程
private slots:
    void on_pushButton_clicked();

private:
    Ui::Widget *ui;
    QTimer * mytimer;
    MyThread * thread;
};
#endif // WIDGET_H
