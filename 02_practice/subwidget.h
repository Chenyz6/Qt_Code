#ifndef SUBWIDGET_H
#define SUBWIDGET_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QPushButton>

class SubWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SubWidget(QWidget *parent = nullptr);

    void sendSlot();

signals://信号必须有signals关键字来声明
        //信号没有返回值，但可以有参数
        //信号就是函数声明 只需声明 无需定义
        //使用 emit mySignal();
        //信号可以重载
    //void mySignal(int, QString);

    void mySignal();
    void mySignal(int, QString);

private:
    QPushButton b;
};

#endif // SUBWIDGET_H
