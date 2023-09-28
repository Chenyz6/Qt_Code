#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建一个3D表面对象

    surface.setFlags(surface.flags() ^ Qt::FramelessWindowHint); // 取消窗口边框
    surface.setWidth(800);
    surface.setHeight(600);

    // 创建一个3D表面数据代理对象
    QtDataVisualization::QSurfaceDataProxy *proxy = new QtDataVisualization::QSurfaceDataProxy();

    // 创建一个3D表面系列对象
    QtDataVisualization::QSurface3DSeries *series1 = new QtDataVisualization::QSurface3DSeries(proxy);
    series1->setDrawMode(QtDataVisualization::QSurface3DSeries::DrawSurface);
    series1->setFlatShadingEnabled(false);
//    series1->setBaseGradient(Qt::white, Qt::gray);

    // 创建一个数据集
    QtDataVisualization::QSurfaceDataArray *dataArray1 = new QtDataVisualization::QSurfaceDataArray();
    QtDataVisualization::QSurfaceDataRow *dataRow;
    for (float x = -10; x <= 10; x += 0.1) {
        dataRow = new QtDataVisualization::QSurfaceDataRow();
        for (float z = -10; z <= 10; z += 0.1) {
            float r = sqrt(x * x + z * z);
            float y = sin(r) / r;
            *dataRow << QVector3D(x, y, z);
        }
        *dataArray1 << dataRow;
    }

    // 设置数据集
    proxy->resetArray(dataArray1);
    surface.addSeries(series1);

    // 显示窗口
    surface.show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

