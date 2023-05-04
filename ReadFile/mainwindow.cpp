#include "mainwindow.h"
#include "ui_mainwindow.h"

#pragma execution_character_set("utf-8")

#include "QFile"
#include "QFileDialog"
#include <QDebug>

short*  quint16Data;
int quint16DataLen;
QString path = "../";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    quint16Data = NULL;
    quint16DataLen = 0;
    char* m;        //文件数据源数据
    QString filename = QFileDialog::getOpenFileName(this,"打开一个数据文件",path,"DAT(*.dat);;TXT(*.txt)");
    setWindowTitle(filename);
    if(filename.isEmpty() == false)
    {
        QFile file(filename);
        bool isok = file.open(QIODevice::ReadOnly);
        if(isok == true)
        {
            QDataStream stream(&file);
            QFileInfo fileInfo(file);
            int FileSize;
            FileSize = fileInfo.size();
            path = fileInfo.path();
            qDebug() << path;
            m = new char[FileSize];
            stream.readRawData(m, FileSize);   // readRawData  读取原始的二进制格式
            short* res = new short[FileSize / sizeof(short)];

            memset(res, 0, FileSize / sizeof(short));

            memcpy(res, m, FileSize);
            quint16Data = res;
            quint16DataLen = FileSize / sizeof(short);
            qDebug() << "数据前三个数：" << quint16Data[0] << quint16Data[1] << quint16Data[2];
            qDebug() << "数据长度：" << quint16DataLen;
//            delete [] res;
//            res = NULL;
        }
        file.close();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->listWidget->clear();
    for(int i = 0; i < 100; i++)
    {
        ui->listWidget->addItem(QString::number(quint16Data[i]));
    }
}
