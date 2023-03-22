#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>
#include <QDataStream>
#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_read_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"open","../","DAT(*.dat);;TXT(*.txt)");
    //限制读取的文件类型
    if(path.isEmpty() == false)
    {
        QFile file(path); // 文件对象
        bool isok = file.open(QIODevice::ReadOnly); // 打开文件  只读方式
        if(isok == true)
        {
            //读文件   默认只识别utf8
            QByteArray array = file.readAll();
            //把文件显示到编辑区
            ui->textEdit->setText(QString(array));
        }

        file.close();   // 关闭文件
        //获取文件信息
        QFileInfo info(path);
//        qDebug() << "文件名字：" << info.fileName();
//        qDebug() << "文件后缀：" << info.suffix();
//        qDebug() << "文件大小：" << info.size();
//        qDebug() << "文件创建时间：" << info.created().toString("yyyy-MM-dd hh:mm:ss");

        cout << "nihao" ;
    }
}



void Widget::on_pushButton_write_clicked()
{
    QString path = QFileDialog::getSaveFileName(this,"save","../","TXT(*.txt)");
    if(path.isEmpty() == false)
    {
        QFile file(path);
        //file.setFileName(path);
        bool isok = file.open(QIODevice::WriteOnly);
        if(isok == true)
        {
            QString str = ui->textEdit->toPlainText();//获取编辑区内容
            //把QString 转换成 QByteArray
            //file.write(str.toUtf8());


            // QString -> c++ string -> char *
            file.write(str.toStdString().data());


            //QString -> QByteArray
//            QString buf = "123";
//            buf.toUtf8();//带中文
//            buf.toLocal8Bit(); //本地编码

//            // char * -> QString
//            char * p = "abc";
//            QString qstring = QString(p);
        }
        file.close();
    }
}









