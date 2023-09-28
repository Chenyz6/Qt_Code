#include "jiemian1.h"
#include "ui_jiemian1.h"

jiemian1::jiemian1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::jiemian1)
{
    ui->setupUi(this);
}

jiemian1::~jiemian1()
{
    delete ui;
}
