#ifndef WIDGET_H
#define WIDGET_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QPushButton>
#include "subwidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public slots:
    void mySlot();
    void changeWin();
    void dealSub();
    void dealSlot(int,QString);


private:
    QPushButton b1;
    QPushButton *b2;
    QPushButton b3;
    SubWidget zi;

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
