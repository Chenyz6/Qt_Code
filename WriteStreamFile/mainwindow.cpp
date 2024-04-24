#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_writestreamfile_clicked()
{
    QFile file("./DemoFile.dat");
    if (!file.open(QIODevice::WriteOnly)) return;
    QDataStream in(&file);
    for(int i = 0; i < 92 * 35 * 10; i++)
    {
        char temp;
        if((rand() % 2) == 0){
            temp = '0';
        }else{
            temp = '1';
        }

        in.writeRawData(&temp, 1);
    }
    file.close();
}

