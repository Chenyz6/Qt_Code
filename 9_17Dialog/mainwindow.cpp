#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMenuBar>
#include<QMenu>
#include<QAction>
#include<QDialog>  //对话框
#include<QDebug>
#include<QMessageBox>
#include<QFileDialog>
#include<QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    resize(500,500);
    QMenuBar *bar = menuBar();
    setMenuBar(bar);
    QMenu *menu = bar->addMenu("对话框");
    QAction *p1 = menu->addAction("模态对话框"); //不能点其他的
    menu->addSeparator();
    QAction *p2 = menu->addAction("非模态对话框");
    connect(p1,&QAction::triggered,
            [=]()
            {
                QDialog dlg;
                dlg.resize(200,200);
                dlg.exec();
                qDebug() << "模态对话框已关闭";
            }
            );
    connect(p2,&QAction::triggered,
            [=]()
            {
                //生命周期 要在头文件创建
                //否则动态分配
//                QDialog *p = new QDialog(this);
//                p->show();   这种方式不好
                  //要是总是按这个按钮 每次都会分配空间
                QDialog *p = new QDialog(this);
                p->setAttribute(Qt::WA_DeleteOnClose);
                p->show();
                dlg.resize(200,200);
                dlg.show();
                qDebug() << "非模态对话框已关闭";
            }
            );

    QAction *p3 = menu->addAction("关于对话框");
    connect(p3,&QAction::triggered,
            [=]()
            {
                QMessageBox::about(this,"about","关于QT");

            }
            );
    QAction *p4 = menu->addAction("问题对话框");
    connect(p4,&QAction::triggered,
            [=]()
            {
                int ret = QMessageBox::question(this,
                                                "question","are you ok?"
                                                ,QMessageBox::Ok |
                                                QMessageBox::Cancel);
                switch(ret)
                {
                case QMessageBox::Ok:
                    qDebug() << "i am ok";
                    break;
                case QMessageBox::Cancel:
                    qDebug() << "i am bad";
                    break;
                default:
                    break;
                }
            }
            );
    QAction *p5 = menu->addAction("文件对话框");
    connect(p5,&QAction::triggered,
            [=]()
            {
                QString path = QFileDialog::getOpenFileName(this,"open",
                                             "../",
                                             "souce(*.cpp *.h);;"
                                             "Text(*.txt);;all(*.*)");
                qDebug() << path;

            }
            );
    QPushButton *button = new QPushButton("nihao", this);
    button->show();
    //button->move(100,100);
    button->setGeometry(100,100,200,200);
}

MainWindow::~MainWindow()
{
    delete ui;
}

