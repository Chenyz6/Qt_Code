#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QDataStream>
#include <QDebug>

QVector<double> xdata;
QVector<double> ydata;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //DealData * zi = new DealData();
    //connect(zi, &DealData::Sendok, this, &MainWindow::deal);
    //ui->widget->graph(0)->setData(xdata, ydata);


    thread = new MyThread(this);
    connect(thread, &MyThread::isDone, this, &MainWindow::DealDone);
    //当点击右上角关闭的时候，关闭线程
    connect(this, &MainWindow::destroy, this, &MainWindow::stopThread);

}


void MainWindow::stopThread()
{
    //停止线程
    thread->quit();
    //等待线程处理完手头工作
    thread->wait();
}

void MainWindow::DealDone()
{
    ui->widget->graph(0)->setData(xTemp, yTemp);
    ui->widget->replot();
    qDebug() << "over" ;
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::deal()
{
    for(int i = 0; i<=(int)G_stDataSave.nDataLen; i+=2)
    {
        qDebug() << G_stDataSave.uint16_Data[i];
        ui->lineEdit->setText(QString(G_stDataSave.uint16_Data[i]));
        xdata << G_stDataSave.uint16_Data[i];
    }
    for(int i = 1; i<= (int)G_stDataSave.nDataLen; i+=2)
    {
        ydata << G_stDataSave.uint16_Data[i];
    }
}

void MainWindow::on_pushButton_clicked()
{
//    Filecontrol* stFilecontrol = new Filecontrol();
//    stFilecontrol->onSelected();


//    QString filename = QFileDialog::getOpenFileName(this,"打开一个数据文件","../","DAT(*.dat);;TXT(*.txt)");
//    if(filename.isEmpty() == false)
//    {
//        QFile file(filename);
//        bool isok = file.open(QIODevice::ReadOnly);
//        if(isok == true)
//        {
//            QDataStream stream(&file);
//            //QByteArray array = file.readAll();
//            QString str;
//            int i;
//            double d;
//            stream >> d;
//            ui->lineEdit->setText(str);
//        }
//        file.close();
//    }

    char* m;        // 文件数据源数据
    QString filename = QFileDialog::getOpenFileName(this,"打开一个数据文件","../","DAT(*.dat);;TXT(*.txt)");
    if(filename.isEmpty() == false)
    {
        QFile file(filename); // QFile 负责文件的 IO 设备接口，即与文件的物理交互
        bool isok = file.open(QIODevice::ReadOnly);
        if(isok == true)
        {
            QDataStream stream(&file); // QDataStream 以数据流的方式读取文件内容或写入文件内容
            QFileInfo fileInfo(file);
            int FileSize;
            FileSize = fileInfo.size();
            qDebug() << "文件大小：" << FileSize;
            m = new char[FileSize];
            stream.readRawData(m, FileSize);   // readRawData  读取原始的二进制格式
            short* res = new short[FileSize / sizeof(short)];
            memset(res, 0, FileSize / sizeof(short));
            memcpy(res, m, FileSize);
            short * quint16Data = res;
            int quint16DataLen = FileSize / sizeof(short);
            qDebug() << "数据前三个数：" << quint16Data[0] << quint16Data[1] << quint16Data[2];
            qDebug() << "数据长度：" << quint16DataLen;
        }
        file.close();
    }
}




void MainWindow::on_pushButton_2_clicked()
{
    for(int i = 0; i<=(int)G_stDataSave.nDataLen - 1; i+=2)
    {
        //qDebug() << G_stDataSave.uint16_Data[i];
        //ui->lineEdit->setText(QString(G_stDataSave.uint16_Data[i]));
        xdata << G_stDataSave.uint16_Data[i];
    }
    for(int i = 1; i<= (int)G_stDataSave.nDataLen - 1; i+=2)
    {
        ydata << G_stDataSave.uint16_Data[i];
    }

    qDebug() << G_stDataSave.nDataLen;

    ui->widget->addGraph();
    ui->widget->graph(0)->setAdaptiveSampling(true);
    ui->widget->graph(0)->setPen(QPen(QColor(0,255,255)));
    ui->widget->setBackground(QColor(37,37,38));
    ui->widget->xAxis->setTickLabelColor(Qt::white);
    ui->widget->yAxis->setTickLabelColor(Qt::white);
    ui->widget->xAxis->setBasePen(QPen(Qt::white));
    ui->widget->yAxis->setBasePen(QPen(Qt::white));
    ui->widget->xAxis->setSubTicks(false);
    ui->widget->yAxis->setSubTicks(false);
    ui->widget->xAxis->setTickPen(QPen(Qt::white));
    ui->widget->xAxis->setTickLengthIn(0);   // 向内延伸0
    ui->widget->yAxis->setTickPen(QPen(Qt::white));
    ui->widget->yAxis->setTickLengthIn(0);
    ui->widget->yAxis->grid()->setZeroLinePen(Qt::NoPen);  // 0刻度线隐藏
    ui->widget->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    //ui->widget->graph(0)->rescaleAxes();
    ui->widget->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom| QCP::iSelectAxes |
    QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->widget->xAxis->setRange(0,100000);
    ui->widget->yAxis->setRange(-5,5);


    thread->start();

}

double* MainWindow::CalSingalFrequency(double* pData, long nDataLen) //获取信号瞬时频率
{
    double* pResult = new double[nDataLen / 2];
    double ti, tr;
    for (long i = 0; i < nDataLen / 2 - 1; i++)
    {
        ti = pData[2 * i] * pData[2 * i + 3] - pData[2 * i + 2] * pData[2 * i + 1];
        tr = pData[2 * i + 2] * pData[2 * i] + pData[2 * i + 3] * pData[2 * i + 1];

        pResult[i] = atan2(ti, tr);
    }
    pResult[nDataLen / 2 - 1] = 0.0;
    return pResult;
}


