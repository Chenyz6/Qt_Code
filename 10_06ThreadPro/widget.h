#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <mythread.h>
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
    void dealsignal();
    void dealdestroy();
private slots:
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();

signals:
    void startthread();//启动子线程的信号
private:
    Ui::Widget *ui;
    MyThread * Mytimer;
    QThread * thread;
};
#endif // WIDGET_H
