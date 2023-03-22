#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this,&MainWindow::starting,gen,&Generate::recv_num);
    connect(gen,&Generate::sendArray,bubble,&BubbleSort::recv_list);
    connect(gen,&Generate::sendArray,this,
        [=](QVector<int> list){
            bubble->start();
            for(int i = 0;i < list.size();i++)
            {
                ui->randList->addItem(QString::number(list[i]));
            }
         });
    connect(bubble,&BubbleSort::finishArray,
            [=](QVector<int> list)
            {
                for(int i = 0;i < list.size(); i++)
                {
                    ui->bubbleList->addItem(QString::number(list[i]));
                }
            });
    connect(this,&MainWindow::destroyed,this,
            [=]()
            {
                gen->quit();
                gen->wait();
                gen->deleteLater();  // delete t1;

                bubble->quit();
                bubble->wait();
                delete bubble;
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_start_clicked()
{
    emit starting(20000);
    gen->start();
}
