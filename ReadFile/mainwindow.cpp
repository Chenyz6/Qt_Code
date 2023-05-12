#include "mainwindow.h"
#include "ui_mainwindow.h"

#pragma execution_character_set("utf-8")

#include "QFile"
#include "QFileDialog"
#include <QDebug>

QVector<double> Waterfall_XData;				//瀑布图x轴数据点数
QVector<double> Waterfall_YData;				//瀑布图y轴数据点数
QList<QVector<double> > value_lofar;			//瀑布图数据
int nWFWndWidth = 0;							//瀑布图宽度

QVector<double> data1;
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
            for(int i = 0;i < quint16DataLen;i++)
            {
                data1 << quint16Data[i];
            }

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

void MainWindow::on_pushButton_3_clicked()  // paint
{

    ui->Waterfall->setBackground(QColor(37, 37, 38));	    //设置背景颜色(黑)
    ui->Waterfall->xAxis->setVisible(false);				//使x轴网格线不可见
    ui->Waterfall->yAxis->setVisible(false);				//使y轴网格线不可见


    m_pColorMap = new QCPColorMap(ui->Waterfall->xAxis, ui->Waterfall->yAxis);
//    m_pColorMap->data()->setSize(1001, 70 + 1);									//设置整个图的x,y点数
//    m_pColorMap->data()->setRange(QCPRange(0, 1001), QCPRange(0, 70 + 1));		//设置x轴和y轴的范围


    m_pColorScale = new QCPColorScale(ui->Waterfall);				//构造一个色条
    m_pColorScale->setType(QCPAxis::atBottom);						//色条数值显示在色条下方
    ui->Waterfall->plotLayout()->addElement(1, 0, m_pColorScale);	//色条显示在瀑布图最下方
    m_pColorScale->setDataRange(QCPRange(0, 100));					//设置色条数值范围
    m_pColorScale->setBarWidth(10);									//设置色条宽度

    QCPColorGradient gradient;										// 色条使用自定义颜色渐变
    gradient.setColorStopAt(0.0, QColor(0, 0, 0));					// 设置色条开始时的颜色(黑
    gradient.setColorStopAt(0.1, QColor(0, 0, 255));				// 蓝
    gradient.setColorStopAt(0.3, QColor(0xcc, 0, 0xff));			// 紫
    gradient.setColorStopAt(0.5, QColor(0xff, 0, 0));				// 红
    gradient.setColorStopAt(0.7, QColor(0xff, 0xff, 0));			// 黄
    gradient.setColorStopAt(0.9, QColor(0, 0xff, 0));				// 绿
    gradient.setColorStopAt(1.0, QColor(255, 255, 255));			// 设置色条结束时的颜色(白

    m_pColorScale->setGradient(gradient);							//将自定义渐变色插入色条
    m_pColorMap->setColorScale(m_pColorScale);						//设置自定义渐近色变化

    ui->Waterfall->yAxis->setRangeReversed(true);					//设置Y轴反向
    m_pColorMap->setInterpolate(true);								//色图平滑变化
    ui->Waterfall->rescaleAxes();									//自适应大小



    // 设置颜色图的数据范围
    int nx = 101; // x 轴方向数据点个数
    int ny = 51; // y 轴方向数据点个数
    m_pColorMap->data()->setSize(nx, ny);
    m_pColorMap->data()->setRange(QCPRange(0, nx), QCPRange(0, 51));
    m_pColorMap->setDataRange(QCPRange(0, 100));

    connect(time,&QTimer::timeout,this,
            [=]()
            {
                if(value_lofar.size()>49)
                {
                    value_lofar.removeLast();  //当lofar累积到了50个，删除最后面的数据，防止绘图溢出绘图区域
                }
                QVector<double> data;
                for(int i  = 0;i<100;i++)
                {
                    data << qrand() % 100;
                }
                value_lofar.prepend(data);//新来的数据一直往前面累加
                for (int i=0;i<value_lofar.size();i++)
                {
                    for(int j=0;j<100;j++)
                    {
                        m_pColorMap->data()->setCell(j,i,value_lofar[i][j]);
                    }
                }
                ui->Waterfall->replot();
            });

    time->start(500);


}
