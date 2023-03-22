#include "widget.h"
#include "ui_widget.h"
#include<QDebug>
#include <QKeyEvent>
//#include<QMouseEvent>
#include<QCloseEvent>
#include<QMessageBox>
#include<QEvent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    timerId = this->startTimer(1000);
    timerId2 = this->startTimer(500);

    connect(ui->pushButton, &MyButton::clicked,
            [=]()
            {
                qDebug() << "按钮被按下";
            }
            );
    //安装过滤器
    ui->label_2->installEventFilter(this);
}

bool Widget::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == ui->label_2)
    {
        //判断事件
        if(e->type() == QEvent::MouseMove)
        {
            QMouseEvent *env = static_cast<QMouseEvent *>(e);
            ui->label_2->setText(QString("Mouse move:(%1,%2)").arg(env->x()).arg(env->y()));
        }
        else
        {
            return QWidget::eventFilter(obj, e);
        }
    }
    else
    {
        return QWidget::eventFilter(obj, e);
    }
}

void Widget::closeEvent(QCloseEvent *e)
{
    int ret = QMessageBox::question(this,"question","是否需要关闭窗口");
    if(ret == QMessageBox::Yes)
    {
        //关闭窗口
        //处理关闭窗口事件，接收事件，事件就不会再往下传递
        e->accept();
    }else
    {
        //不关闭窗口
        //忽略事件，事件继续给父组件传递
        e->ignore();
    }
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "打印了父组件";
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    qDebug() << e->key();
    if(e->key() == Qt::Key_A)
    {
        qDebug() << "Qt::Key_A";
    }
}

bool Widget::event(QEvent *e)
{
//    //事件分发
//    switch(e->type())
//    {
//    case QEvent::MouseMove:
//        closeEvent(e);
//        break;
//    case QEvent::MouseMove:
//        mouseMoveEvent(e);
//        break;
//    }
    if(e->type() == QEvent::Timer)
    {
        //干掉定时器
        //如果返回TRUE，事件停止传播
        QTimerEvent *env = static_cast<QTimerEvent *>(e);
        timerEvent(env);  //类型不一样 需要类型转换
        return true;
    }
    else if(e->type() == QEvent::KeyPress)
    {
        QKeyEvent *env = static_cast<QKeyEvent *>(e);
        if(env->key() == Qt::Key_B)
        {
            return QWidget::event(e);

        }
        return true;
    }
    else
    {
        return QWidget::event(e);
    }

}

void Widget::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == this->timerId)
    {
        static int sec = 0;
        sec++;
        ui->label->setText(QString("<center><h1>time out %1</h1></center>")
                                   .arg(sec));
        if(5 == sec)
        {
            //停止定时器
            killTimer(this->timerId);
        }
    }
    else if(e->timerId() == this->timerId2)
    {
        static int sec = 0;
        ui->label_2->setText(QString("<center><h1>time out %1</h1></center>")
                                   .arg(sec++));
    }




}

Widget::~Widget()
{
    delete ui;
}

