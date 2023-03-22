#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
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
    void paintEvent(QPaintEvent *);
    //重写绘图事件 虚函数
    //如果在窗口绘图，必须放在绘图事件里实现
    //绘图事件内部自动调用，窗口需要重绘的时候（状态改变）
private slots:
    void on_pushButton_clicked();

private:
    Ui::Widget *ui;
    int x;
};
#endif // WIDGET_H
