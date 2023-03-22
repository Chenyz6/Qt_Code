#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

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
    //重写绘图事件
    //如果要给窗口绘图 一定要在这里实现
    void paintEvent(QPaintEvent *);
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
