#ifndef MYLABLE_H
#define MYLABLE_H

#include <QLabel>
class MyLable : public QLabel
{
    Q_OBJECT
public:
    explicit MyLable(QWidget *parent = nullptr);

protected:
    //鼠标点击事件
    void mousePressEvent(QMouseEvent *ev);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *ev);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *ev);
    //进入窗口区域
    void enterEvent(QEvent *e);
    //离开窗口区域
    void leaveEvent(QEvent *e);
signals:

};

#endif // MYLABLE_H
