#ifndef ADDWIDGET_H
#define ADDWIDGET_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QPushButton>

class addwidget : public QWidget
{
    Q_OBJECT
public:
    explicit addwidget(QWidget *parent = nullptr);
private:
    QPushButton e;
public:
    void addconvert();
signals:
    void slot();

};

#endif // ADDWIDGET_H
