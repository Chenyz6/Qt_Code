#include "jiemian4.h"
#include "ui_jiemian4.h"
#include "qcustomplot.h"
#include <QPen>
#include <QBrush>
jiemian4::jiemian4(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::jiemian4)
{
    ui->setupUi(this);
    setWindowTitle("界面4");
    ui->statusbar->setSizeGripEnabled(false);
    //表示是否在对话窗右下角显示抓痕（英文grip），该抓痕表示可以通过点击拖拽对窗口进行大小调整
    ui->statusbar->setStyleSheet("color:rgb(255, 0, 0)");

    timer = new QTimer;
    connect(timer, &QTimer::timeout, this, &jiemian4::StatusBarTime);
    timer->start(1000);			//启动时间定时器

    ui->widget->xAxis->setRange(-1,1);
    ui->widget->xAxis->setLabel("x");
    ui->widget->yAxis->setRange(0,1);
    ui->widget->yAxis->setLabel("y");
    //设置框选放大
    ui->widget->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
    //设置框选边框为虚线
    ui->widget->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));
    //设置框选范围半透明浅蓝
    ui->widget->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));


    InitSpectrum();	//初始化频谱图

}

void jiemian4::InitSpectrum()
{
    //添加一条曲线：每一条曲线都必须独占一个graph，新建graph可通过addGraph()来实现
    // 生成数据，画出的是抛物线
//    QVector<double> x(101), y(101); //初始化向量x和y
//    for (int i=0; i<101; ++i)
//    {
//      x[i] = i/50.0 - 1; // x范围[-1,1]
//      y[i] = x[i]*x[i]; // y=x*x
//    }
//    ui->widget->addGraph();//添加数据曲线（一个图像可以有多个数据曲线）
//    // graph(0);可以获取某个数据曲线（按添加先后排序）
//    // setData();为数据曲线关联数据
//    ui->widget->graph(0)->setData(x, y);
//    ui->widget->graph(0)->setName("示例1");// 设置图例名称
//    ui->widget->legend->setVisible(true); // 显示图例  legend 图例


    //每条曲线都会独占一个graph()
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue)); // 曲线的颜色
    ui->customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // 曲线与X轴包围区的颜色

    ui->customPlot->addGraph();//添加graph等价于添加一条新曲线
    ui->customPlot->graph(1)->setPen(QPen(Qt::red)); // 曲线的颜色
    // 生成模拟数据点 (x-y0 第一条曲线, x-y1为第2条曲线):
    QVector<double> x(251), y0(251), y1(251);
    for (int i=0; i<251; ++i)
    {
       x[i] = i;
       y0[i] = qExp(-i/150.0)*qCos(i/10.0); // 第一条曲线：y0衰减cos
       y1[i] = qExp(-i/150.0);              // 第二条曲线：y1衰减指数
    }
    // 边框右侧和上侧均显示刻度线，但不显示刻度值:
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);
    // 使上下两个X轴的范围总是相等，使左右两个Y轴的范围总是相等
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
    // 把已存在的数据填充进graph的数据区
    ui->customPlot->graph(0)->setData(x, y0);
    ui->customPlot->graph(1)->setData(x, y1);
    //自动调整XY轴的范围，以便显示出graph(0)中所有的点（下面会单独讲到这个函数）
    ui->customPlot->graph(0)->rescaleAxes();
    //自动调整XY轴的范围，以便显示出graph(1)中所有的点
    ui->customPlot->graph(1)->rescaleAxes(true);
    //显示图例
    ui->customPlot->graph(0)->setName("示例1");// 设置图例名称
    ui->customPlot->legend->setVisible(true); // 显示图例  legend 图例
    ui->customPlot->graph(1)->setName("示例2");// 设置图例名称
    ui->customPlot->legend->setVisible(true); // 显示图例  legend 图例
    // 支持鼠标拖拽轴的范围、滚动缩放轴的范围，左键点选图层（每条曲线独占一个图层）
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    //QCP::iRangeDrag：可以拖动
    //QCP::iRangeZoom：可以缩放
    //QCP::iSelectLegend：图像选择，没用到，不知道效果
    //QCP::iSelectPlottables：图层可以选择，实测就是可以选中曲线
    //QCP::iSelectAxes：坐标轴选择
    // 立即刷新图像
    ui->customPlot->replot();
}

void jiemian4::StatusBarTime()
{
    QString Stime;
    QDateTime Timedata = QDateTime::currentDateTime(); //获取当前时间
    Stime = Timedata.toString("yyyy-MM-dd hh:mm:ss a dddd t");
    LabelTime->setText(Stime);
    ui->statusbar->addWidget(LabelTime);//临时信息（持续显示）
}

jiemian4::~jiemian4()
{
    delete ui;
}

void jiemian4::on_pushButton_clicked()
{
    ui->customPlot->graph(0)->rescaleAxes();
    //自动调整XY轴的范围，以便显示出graph(1)中所有的点
    ui->customPlot->graph(1)->rescaleAxes(true);
    ui->customPlot->replot();
}
