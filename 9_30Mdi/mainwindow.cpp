#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCompleter>  //QCompleter能实现自动填充功能,方便用户输入,提升用户的体验
#include "jiemian4.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //resize(900,700);
    //ui->statusbar->setSizeGripEnabled(false);
    ui->mdiArea->addSubWindow(&jiemian1, Qt::CustomizeWindowHint);
    // Qt::CustomizeWindowHint 摆脱窗体顶部的默认按钮以及窗体的边框
    jiemian1.setAttribute(Qt::WA_DeleteOnClose);//设置widget窗口的属性为关闭自动释放内存
    jiemian1.show();    //customize 定制
    jiemian1.showFullScreen();

    ui->mdiArea->addSubWindow(&jiemian2, Qt::CustomizeWindowHint);
    jiemian2.setAttribute(Qt::WA_DeleteOnClose);
    jiemian2.show();
    jiemian2.showFullScreen();

    ui->mdiArea->addSubWindow(&jiemian3, Qt::CustomizeWindowHint);
    jiemian3.setAttribute(Qt::WA_DeleteOnClose);
    jiemian3.show();
    jiemian3.showFullScreen();

    ui->mdiArea->addSubWindow(&jiemian4,Qt::CustomizeWindowHint);
    jiemian4.setAttribute(Qt::WA_DeleteOnClose);
    jiemian4.show();


    QList<QMdiSubWindow* > subWindowList = ui->mdiArea->subWindowList();
    qDebug() << subWindowList.size();
    ui->mdiArea->setActiveSubWindow(subWindowList[0]);

    QStringList list;
    list << "apple" << "animal" << "a" << "an" << "abstract" << "car" << "city" << "chenyuze";
    QCompleter *com = new QCompleter(list,this);
    com->setCaseSensitivity(Qt::CaseInsensitive);  //不区分大小写
    ui->lineEdit->setCompleter(com);

    QStringList item;
    item.append("陈");
    item << "与" << "泽" ;
    ui->comboBox_2->addItems(item);  // 下拉列表框

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionTab_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    // TabbedView在选项卡栏中显示带有选项卡的子窗口
}

void MainWindow::on_action_5_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    // SubWindowView显示带有窗口框架的子窗口
    ui->mdiArea->cascadeSubWindows();
    // cascadeSubWindows()级联子窗口
}

void MainWindow::on_action_9_triggered()
{
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->mdiArea->tileSubWindows();
    // tileSubWindows()平铺子窗口
}

