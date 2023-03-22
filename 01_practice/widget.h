#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>  //窗口控件基类

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget  //继承
{
    Q_OBJECT //信号与槽的时候需要

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
