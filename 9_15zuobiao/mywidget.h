#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#pragma execution_character_set("utf-8")
QT_BEGIN_NAMESPACE
namespace Ui { class MyWidget; }
QT_END_NAMESPACE

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    MyWidget(QWidget *parent = nullptr);
    ~MyWidget();

private:
    Ui::MyWidget *ui;
};
#endif // MYWIDGET_H
