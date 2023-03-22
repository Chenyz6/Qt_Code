#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // 3.将任务对象移动到某个线程中
    gen->moveToThread(t1);
    bubble->moveToThread(t2);

    connect(this,&MainWindow::starting,gen,&Generate::working);
    connect(gen,&Generate::sendArray,bubble,&BubbleSort::working);
    connect(gen,&Generate::sendArray,this,
        [=](QVector<int> list){
            t2->start();
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
                t1->quit();
                t1->wait();
                t1->deleteLater();  // delete t1;

                t2->quit();
                t2->wait();
                delete t2;
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_start_clicked()
{
    emit starting(20000);
    t1->start();
}
