#include "mybutton.h"
#include<QMouseEvent>
#include<QDebug>
MyButton::MyButton(QWidget *parent) : QPushButton(parent)
{

}

void MyButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        qDebug() << "按下的是左键";
        //事件接收后，就不会往下传递
        e->ignore();//忽略 事件继续往下传递
                    //事件传递给了父组件，不是给父类（基类）
                    //主要不是在这用
    }else
    {
          //不作处理
        QPushButton::mousePressEvent(e);
        //事件的忽略，就会继续往下传递
    }

}
