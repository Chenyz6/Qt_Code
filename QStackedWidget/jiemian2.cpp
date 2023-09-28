#include "jiemian2.h"
#include "ui_jiemian2.h"

jiemian2::jiemian2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::jiemian2)
{
    ui->setupUi(this);
}

jiemian2::~jiemian2()
{
    delete ui;
}
