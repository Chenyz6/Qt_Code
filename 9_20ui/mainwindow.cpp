#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCompleter>  //QCompleter能实现自动填充功能,方便用户输入,提升用户的体验
#include <QStringList>
#include <QMovie>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);  //放在后面进行编辑
    ui->mybutton->setText("123");
    //connect(ui->mybutton,&QPushButton::clicked,this,&MainWindow::close);


    QString str = ui->lineEdit->text();  //获取内容
    qDebug() << str;

    //设置内容
    ui->lineEdit->setText("123456");

    //设置内容显示方式
    ui->lineEdit->setTextMargins(15,0,0,0);//以像素点为单位

    //设置内容显示方式
    //ui->lineEdit->setEchoMode(QLineEdit::Password);

    QStringList list;
    list << "hello" << "How are you " << "hehe";
    QCompleter *com = new QCompleter(list,this);
    com->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit->setCompleter(com);

    //Qlabel
    ui->labelText->setText("123");
    //设置图片
    //ui->labelImage->setPixmap(QPixmap(":/image/2.png"));
    //让图片自适应label大小
    //ui->labelImage->setScaledContents(true);
    //创建动画
    //QMovie *movie = new QMovie(":/image/4.gif");
    //设置动画
   // ui->labelGif->setMovie(movie);
    //启动动画
    //movie->start();
    ui->labelGif->setScaledContents(true);
    //设置HTML
    ui->labelUrl->setText("<h1><a href=\"https://www.baidu.com\">百度一下</a></h1>");
    ui->labelUrl->setOpenExternalLinks(true);
    //数码管
    ui->lcdNumber->display(123);
    //进度条
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(200);
    ui->progressBar->setValue(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_8_clicked()
{
    static int i = 0;
    ui->stackedWidget->setCurrentIndex(++i % 4);
    //setCurrentIndex  设置当前的索引值


}
