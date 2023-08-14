#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(timer, &QTimer::timeout, this, &MainWindow::timeout);  // 连接定时器显示时间
}
int temp = 0;
void MainWindow::timeout()  // 定时器槽函数
{
    QBarDataRow *dataBar = new QBarDataRow;

//    for(int i = 0; i < 1024; i++){
//        *dataBar << 10 * log(spectrum[i]) + 70;
//    }
#if 1
    for(float i = 0; i < 1024; i++)
    {
        if(temp >= 15)
        {
            if(i == 0){
                *dataBar << 100;
                continue;
            }
            if(i >= 600 && i <= 750){
                *dataBar << 60 + rand() % 10;
            }else{
                *dataBar << 50 + rand() % 15;
            }
        }else{
            if(i == 0){
                *dataBar << 100;
                continue;
            }
            if(i >= 400 && i <= 600){
                *dataBar << 50 + rand() % 10;
            }else{
                *dataBar << 30 + rand() % 20;
            }
        }
    }
#endif
//    seriesBar->dataProxy()->addRow(dataBar);
    seriesBar->dataProxy()->insertRow(temp,dataBar);
    temp++;
    if(temp == 30)
        temp = 0;
    bars.addSeries(seriesBar);
}

void MainWindow::on_pushButton_clicked()    // 显示3d平面图
{
    // 创建一个3D表面对象
    flatSurface.setFlags(flatSurface.flags() ^ Qt::FramelessWindowHint); // 取消窗口边框 显示主界面菜单栏
    flatSurface.setWidth(800);
    flatSurface.setHeight(600);

    flatSurface.axisX()->setTitle("X轴");
    flatSurface.axisX()->setTitleVisible(true);
    flatSurface.axisX()->setRange(0,100);
    flatSurface.axisY()->setTitle("Y轴");
    flatSurface.axisY()->setTitleVisible(true);
    flatSurface.axisY()->setRange(0,100);
    flatSurface.axisZ()->setTitle("Z轴");
    flatSurface.axisZ()->setTitleVisible(true);
    flatSurface.axisZ()->setRange(0,50);

    flatSurface.activeTheme()->setGridEnabled(true);   // 坐标子网格线

    // 创建一个3D表面数据代理对象
    QtDataVisualization::QSurfaceDataProxy *proxy = new QtDataVisualization::QSurfaceDataProxy();

    // 创建一个3D表面系列对象
    QtDataVisualization::QSurface3DSeries *series = new QtDataVisualization::QSurface3DSeries(proxy);
    series->setDrawMode(QtDataVisualization::QSurface3DSeries::DrawSurfaceAndWireframe);   // 默认不写这个是带网格线的
    series->setFlatShadingEnabled(false);  // 是否启用曲面平铺阴影

    // 创建一个数据集
    QtDataVisualization::QSurfaceDataArray *dataArray = new QtDataVisualization::QSurfaceDataArray();
    QtDataVisualization::QSurfaceDataRow *dataRow;
//    for (float x = -10; x <= 10; x += 0.1) {
//        dataRow = new QtDataVisualization::QSurfaceDataRow();
//        for (float z = -10; z <= 10; z += 0.1) {
//            float r = sqrt(x * x + z * z);
//            float y = sin(r) / r;
//            *dataRow << QVector3D(x, y, z);
//        }
//        *dataArray << dataRow;
//    }

    for (float n = 0; n < 50; n += 0.1) {
        dataRow = new QtDataVisualization::QSurfaceDataRow();
        for (float m = 0; m < 100; m ++) {
            if(m>=40 && m<=60){
                *dataRow << QVector3D(m, 50+qrand() % 5, n);
            }else{
                *dataRow << QVector3D(m, 10+qrand() % 5, n);
            }
        }
        *dataArray << dataRow;
    }

//    dataRow = new QtDataVisualization::QSurfaceDataRow();
//    for (float m = 0; m < 100; m ++)
//    {
//        *dataRow << QVector3D(m, qrand() % 100, 5);
//    }

//    *dataArray << dataRow;
//    dataRow = new QtDataVisualization::QSurfaceDataRow();
//    for (float m = 0; m < 100; m ++)
//        *dataRow << QVector3D(m, qrand() % 100, 40);

//    *dataArray << dataRow;

    // 设置数据集
    proxy->resetArray(dataArray);
    flatSurface.addSeries(series);

    // 显示窗口
    flatSurface.show();
}

void MainWindow::on_pushButton_2_clicked()  // 显示3d柱状图
{
    bars.setFlags(bars.flags() ^ Qt::FramelessWindowHint); // 取消窗口边框 显示主界面菜单栏
    bars.setWidth(800);
    bars.setHeight(600);
    bars.setBarSpacing(QSizeF(0,50));  // QSizeF为X和Z维度上的条形图间距
    seriesBar->setColorStyle(Q3DTheme::ColorStyleRangeGradient);  // 根据对象的高度及其在 Y 轴上的位置，使用完整渐变的一部分为对象着色
//    seriesBar->setBaseColor(Q3DTheme::ThemeStoneMoss);

    // 设置自定义颜色
    QLinearGradient gradient;
    gradient.setColorAt(0.0, QColor(0, 0, 0, 150));					// 设置色条开始时的颜色(黑
    gradient.setColorAt(0.1, QColor(0, 0, 255, 150));				// 蓝
    gradient.setColorAt(0.3, QColor(0xcc, 0, 0xff, 150));			// 紫
    gradient.setColorAt(0.5, QColor(0xff, 0, 0, 150));				// 红
    gradient.setColorAt(0.7, QColor(0xff, 0xff, 0, 150));			// 黄
    gradient.setColorAt(0.9, QColor(0, 0xff, 0, 150));				// 绿
    gradient.setColorAt(1.0, QColor(255, 255, 255, 150));			// 设置色条结束时的颜色(白
//    seriesBar->setBaseGradient(gradient);


    bars.rowAxis()->setRange(0, 30);  //Z轴(行)的范围，最多可包含n行，也就是在Z轴方向上最多有n个数据点
    bars.columnAxis()->setRange(0, 1000); //X轴(列)的范围，最多可包含n列
//    QBar3DSeries *seriesBar = new QBar3DSeries;

//    for(int j = 0;j < 30; j++)
//    {
//        QBarDataRow *dataBar = new QBarDataRow;
//        for(float i = 0; i < 1000; i++)
//        {
//            if(i == 0){
//                *dataBar << 100;
//                continue;
//            }
//            if(i >= 400 && i <= 600){
//                *dataBar << 50 + rand() % 10;
//            }else{
//                *dataBar << rand() % 30;
//            }
//        }
//        seriesBar->dataProxy()->addRow(dataBar);
//    }

//    bars.addSeries(seriesBar);


    for(float i = 0; i < 30; i++)
    {
        QBarDataRow *dataBar = new QBarDataRow;
        for(float i = 0; i < 1024; i++)
        {
            *dataBar << 0;
        }
        seriesBar->dataProxy()->addRow(dataBar);
    }
    bars.addSeries(seriesBar);


    timer->start(15);

    bars.show();

}


MainWindow::~MainWindow()
{
    delete ui;
}
