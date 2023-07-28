#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("QProcess Receive");

    QTimer *reconnect_timer = new QTimer(this);
    connect(reconnect_timer,&QTimer::timeout,this,&MainWindow::reconect_to_server);

    m_socket = new QLocalSocket(this);

    connect(m_socket,SIGNAL(error(QLocalSocket::LocalSocketError)),this,SLOT(error_proc(QLocalSocket::LocalSocketError))); //数据接收
    connect(m_socket,&QLocalSocket::connected,this,&MainWindow::connect_success);  //数据接收
    connect(m_socket,&QLocalSocket::disconnected,this,&MainWindow::disconnect_from_server);  //连接断开
    connect(m_socket,&QLocalSocket::readyRead,this,&MainWindow::rcv_data);  //数据接收
    m_socket->connectToServer("local_server");  //连接到服务器
}

void MainWindow::rcv_data() //收到数据
{
    QByteArray data = m_socket->readAll();
    ui->textBrowser->insertPlainText(QString(data));
    ui->listWidget->addItem("--------------------------");
    QVector<double> temp;
    for(int i = 0; i< data.size(); i++)
    {
        temp << data[i];
        ui->listWidget->addItem(QString::number(temp[i]));
    }
}

void MainWindow::connect_success()
{
    ui->textBrowser->insertPlainText("连接服务端成功!");
}

void MainWindow::disconnect_from_server()
{
    ui->textBrowser->insertPlainText("连接断开！");
}

void MainWindow::reconect_to_server()
{
    reconnect_timer->stop();
    ui->textBrowser->insertPlainText("连接重试中...");
    if(m_socket)
        m_socket->connectToServer("myserver");
}

void MainWindow::error_proc(QLocalSocket::LocalSocketError state)
{
    ui->textBrowser->insertPlainText("连接服务器失败!");
    ui->textBrowser->insertPlainText(QString("错误码:%1").arg(QString::number(state)));
    m_socket->close();
    reconnect_timer->start(1000);  //1s后重连
}


MainWindow::~MainWindow()
{
    delete ui;
}


