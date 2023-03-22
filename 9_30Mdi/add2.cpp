#include "add2.h"
#include "ui_add2.h"

add2::add2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::add2)
{
    ui->setupUi(this);
    setWindowTitle("界面2");
    ui->lcdNumber_2->display(521);
}

add2::~add2()
{
    delete ui;
}
