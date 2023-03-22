#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QEvent>
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
protected:
    //键盘按下事件
    void keyPressEvent(QKeyEvent *);
    //计时器事件
    void timerEvent(QTimerEvent *);
    //鼠标点击事件
    void mousePressEvent(QMouseEvent *e);
    //关闭事件
    void closeEvent(QCloseEvent *e);
    //重写event事件
    bool event(QEvent *);
    //事件过滤器
    bool eventFilter(QObject *obj, QEvent *e);
private:
    Ui::Widget *ui;
    int timerId;
    int timerId2;
};
#endif // WIDGET_H
