#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include "addwidget.h"
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
    void convert();
    void convert1();
private:
    QPushButton c;
    QPushButton d;
public:
    addwidget zi;
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
