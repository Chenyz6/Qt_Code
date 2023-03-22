#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QWidget>
#include <QPushButton>

#pragma execution_character_set("utf-8")
class MyButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyButton(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *e);
signals:

};

#endif // MYBUTTON_H
