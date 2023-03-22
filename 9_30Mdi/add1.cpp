#include "add1.h"
#include "ui_add1.h"

add1::add1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::add1)
{
    ui->setupUi(this);
    setWindowTitle("界面1");
    text();
}

add1::~add1()
{
    delete ui;
}

void add1::text()
{
    ui->widget->xAxis->setRange(0, 300);
    ui->widget->yAxis->setRange(0,400);
}
