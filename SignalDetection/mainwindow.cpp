#include "mainwindow.h"
#include "ui_mainwindow.h"

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

// 图像检测
void MainWindow::on_pushButton_ImageDetection_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"打开一个音频文件","../","JPG(*.jpg);;PNG(*.png);;所有文件(*.*)");
    if(filename.isEmpty() == false)
    {
        QFile file(filename);
        bool isok = file.open(QIODevice::ReadOnly);
        if(isok == true) // 如果文件打开
        {
            QDataStream stream(&file); // QDataStream 以数据流的方式读取文件内容或写入文件内容
            QFileInfo fileInfo(file);
            qDebug() << QString("文件大小:%1B(Byte)--%2KB--%3MB--%4GB")
                        .arg((double)fileInfo.size())
                        .arg((double)fileInfo.size()/1024)
                        .arg((double)fileInfo.size()/1024/1024)
                        .arg((double)fileInfo.size()/1024/1024/1024);
            qDebug() << QString("文件路径:%1").arg(fileInfo.path());
            qDebug() << QString("文件名称:%1").arg(fileInfo.fileName());
            qDebug() << QString("创建时间:%1").arg(fileInfo.created().toString("yyyy-MM-dd hh:mm:ss"));
            qDebug() << QString("修改时间:%1").arg(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
        }else{
            QMessageBox::warning(0, "Warning", "文件打开失败!");
            return;
        }
        file.close();
    }
}

// 视频检测
void MainWindow::on_pushButton_VedioDection_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"打开一个视频文件","../","MP4(*.mp4);;所有文件(*.*)");
    if(filename.isEmpty() == false)
    {
        QFile file(filename);
        bool isok = file.open(QIODevice::ReadOnly);
        if(isok == true) // 如果文件打开
        {
            QDataStream stream(&file); // QDataStream 以数据流的方式读取文件内容或写入文件内容
            QFileInfo fileInfo(file);
            qDebug() << QString("文件大小:%1B(Byte)--%2KB--%3MB--%4GB")
                        .arg((double)fileInfo.size())
                        .arg((double)fileInfo.size()/1024)
                        .arg((double)fileInfo.size()/1024/1024)
                        .arg((double)fileInfo.size()/1024/1024/1024);
            qDebug() << QString("文件路径:%1").arg(fileInfo.path());
            qDebug() << QString("文件名称:%1").arg(fileInfo.fileName());
            qDebug() << QString("创建时间:%1").arg(fileInfo.created().toString("yyyy-MM-dd hh:mm:ss"));
            qDebug() << QString("修改时间:%1").arg(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
        }else{
            QMessageBox::warning(0, "Warning", "文件打开失败!");
            return;
        }
        file.close();
    }
}

