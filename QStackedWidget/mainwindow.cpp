#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    jm1 = new jiemian1();
    qDebug() << "页面索引: " << ui->stackedWidget->addWidget(jm1);
    jm2 = new jiemian2();
    qDebug() << "页面索引: " << ui->stackedWidget->addWidget(jm2);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked() // 界面1
{
    ui->stackedWidget->setCurrentIndex(0);
    qDebug() << "界面1显示";
}

void MainWindow::on_pushButton_2_clicked() // 界面2
{
    ui->stackedWidget->setCurrentIndex(1);
    qDebug() << "界面2显示";
}

void MainWindow::on_pushButton_3_clicked() // 界面3
{
    ui->stackedWidget->setCurrentIndex(2);
    qDebug() << "界面3显示";
}

void MainWindow::on_pushButton_4_clicked() // 界面4
{
    ui->stackedWidget->setCurrentIndex(3);
    qDebug() << "界面4显示";
}
