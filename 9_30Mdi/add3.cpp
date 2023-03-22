#include "add3.h"
#include "ui_add3.h"
#include <QWidget>
add3::add3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::add3)
{
    ui->setupUi(this);
    setWindowTitle("界面3");
    text();

}

add3::~add3()

{
    delete ui;
}

void add3::text()
{
    ui->widget->xAxis->setRange(0,500);
    ui->widget->xAxis->setLabel("x");
    ui->widget->yAxis->setRange(100,400);
    ui->widget->yAxis->setLabel("y");
    //设置框选放大
    ui->widget->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
    //设置框选边框为虚线
    ui->widget->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));
    //设置框选范围半透明浅蓝
    ui->widget->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));

}
