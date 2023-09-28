#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    groupBox = new Q3DSurface;
    groupBox->axisX()->setTitle("X轴");
    groupBox->axisY()->setTitle("Y轴");
    groupBox->axisZ()->setTitle("Z轴");
    groupBox->axisY()->setRange(-5,10);
    groupBox->axisX()->setRange(-11,11);
    groupBox->axisZ()->setRange(-11,11);
    groupBox->activeTheme()->setGridEnabled(false);

    //将三维图与数据进行绑定
    QSurfaceDataProxy * proxy = new QSurfaceDataProxy;
    series = new QSurface3DSeries(proxy);
    groupBox->addSeries(series);
    //将三维图在组中显示，布局如果需要的话，在添加一个 #include <QBoxLayout>
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(createWindowContainer(groupBox));
    ui->widget->setLayout(vLayout);
    dataArray = new QSurfaceDataArray;


    int N = 41;
    dataArray->reserve(N);
    float x=-10,y,z;
    int i,j;
    for(i = 1;i <= N;i++){
         QSurfaceDataRow *newRow = new QSurfaceDataRow(N);
         y = -10;
         int index = 0;
         for(j=1;j<=N;j++){
             z=qSqrt(x*x+y*y);
             if(z!=0)
                 z=10*qSin(z)/z;
             else
                 z=10;
             (*newRow)[index++].setPosition(QVector3D(x,z,y));
             y=y+0.5;
             *dataArray << newRow;
         }
         x=x+0.5;
         *dataArray<<newRow;
    }
    series->dataProxy()->resetArray(dataArray);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    dataArray->clear();
    groupBox->axisY()->setRange(-5,300);
    int N = 41;
    dataArray->reserve(N);
    float x=-10,y,z;
    int i,j;
    for(i = 1;i <= N;i++){
         QSurfaceDataRow *newRow = new QSurfaceDataRow(N);
         y = -10;
         int index = 0;
         for(j=1;j<=N;j++){
             z=qSqrt(x*x+y*y);
             if(z!=0)
                 z=10*qSin(z)/z;
             else
                 z=10;
             (*newRow)[index++].setPosition(QVector3D(x,z,y));
             y=y+0.5;
             *dataArray << newRow;
         }
         x=x+0.5;
         *dataArray<<newRow;
    }
    series->dataProxy()->resetArray(dataArray);

}

void MainWindow::on_pushButton_2_clicked()
{
    dataArray->clear();
    int N = 41;
    dataArray->reserve(N);
    float x=-10,y,z;
    int i,j;
    for(i = 1;i <= N;i++){
         QSurfaceDataRow *newRow = new QSurfaceDataRow(N);
         y = -10;
         int index = 0;
         for(j=1;j<=N;j++){
             z=qSqrt(x*x+y*y);
             if(z!=0)
                 z=10*qSin(z)/z;
             else
                 z=10;
             (*newRow)[index++].setPosition(QVector3D(x,z,y));
             y=y+0.5;
             *dataArray << newRow;
         }
         x=x+0.5;
         *dataArray<<newRow;
    }
    series->dataProxy()->resetArray(dataArray);
}
