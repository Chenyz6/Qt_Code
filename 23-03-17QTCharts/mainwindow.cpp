#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCharts>
#include <QLineSeries>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /*
      * LineChart Example
      */
     //添加一个QLineSeries实例
     QLineSeries *lineseries = new QLineSeries();
     //添加数据（点），有两种方式
       //参数分别为横纵坐标

//     lineseries->append(3, 8);
//     lineseries->append(0, 4);
//     lineseries->append(2, 4);
//     lineseries->append(7, 4);
     *lineseries << QPointF(14, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);
     //添加一个QChart实例，为了展示数据
     QChart *chart = new QChart();
     chart->legend()->hide();  //隐藏图例
     chart->addSeries(lineseries);
     chart->createDefaultAxes();
     chart->setTitle("Simple line chart example");
     //添加一个QChartView图例
     QChartView *chartView = new QChartView(chart);
     chartView->setRenderHint(QPainter::Antialiasing);  //呈现方式
     //显示
     setCentralWidget(chartView);
     //this->setCentralWidget(ui->widget);
     ui->scrollArea->setWidget(chartView);
     this->resize(400, 300);
     this->show();

}

MainWindow::~MainWindow()
{
    delete ui;
}

