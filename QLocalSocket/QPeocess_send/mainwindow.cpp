#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("QProcess Send");

    m_server = new QLocalServer(this);
    //myserver为服务端名称,类似于IP+PORT,客户端连接需要与其保持一致
    ui->listWidget->addItem("简称名称:local_server");
    if( m_server->listen("local_server") ) //监听
    {
       connect(m_server,&QLocalServer::newConnection,this,&MainWindow::new_connection);
       printf("listen success!!\n");
    }
    else
       printf("listen fail\n");
//       m_server->removeServer("local_server");   //如果SERVER已经存在，则需删除

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QProcess * process = new QProcess;
    process->start("E:/Zstu320/QT_Content/Qt_CodeDebug/QLocalSocket/QProcess_receive/release/QProcess_receive.exe"); // Replace "receiver_app" with the name of the receiving application or its path
//    process->waitForStarted();
//    process.closeWriteChannel();
//    process.waitForFinished();
}

void MainWindow::new_connection()
{
    ui->listWidget->addItem("已有另一个exe连接此进程！");
    QLocalSocket *newsocket = m_server->nextPendingConnection();  //获取连接上的客户端句柄
    connect(newsocket, &QLocalSocket::readyRead, this, &MainWindow::recv_data); //关联数据接收槽函数

#define my_size 8

    short * sh = new short[my_size];
    memset(sh, 0, my_size * 2);
    for(int i = 0; i< my_size; i++)
    {
        sh[i] = qrand() % 100;
        ui->listWidget_2->addItem(QString::number(sh[i]));
    }

    char * c = new char[my_size * 2];
    memset(c, 0, my_size * 2);
//    for(int i = 0; i< 100; i++)
//    {
//        c[i] = qrand() % 100;
//    }

    memcpy(c, sh, my_size * 2);

    newsocket->write(c,my_size * 2);
}

void MainWindow::recv_data()
{
    // 取得是哪个localsocket可以读数据了
    QLocalSocket *local = static_cast<QLocalSocket *>(sender());
    if (!local)
        return;

    QByteArray rcv_data = local->readAll();
    qDebug()<<"rcv_data:"<<rcv_data;

    if(is_first_connect(local))
    {
        local->write(QString("欢迎连接myserver!!").toUtf8());
        ui->listWidget->addItem(QString("这个socket是首次连接"));
    }
    else
    {
        local->write(rcv_data);
        ui->listWidget->addItem(QString("发送数据:%1").arg(QString(rcv_data)));
    }
}


bool MainWindow::is_first_connect(QLocalSocket *newsocket)  //是否为首次连接
{
    int len = local_sockets.length();
    for(int i=0; i<len; i++)
    {
        if(newsocket == (QLocalSocket *)local_sockets.at(i))
            return false;
    }
    local_sockets.append(newsocket);
    return true;
}

void MainWindow::on_pushButton_2_clicked()
{
    char* m;        //文件数据源数据
    QString filename = QFileDialog::getOpenFileName(this,"打开一个数据文件","../","DAT(*.dat);;TXT(*.txt)");
    if(filename.isEmpty() == false)
    {
        QFile file(filename);
        bool isok = file.open(QIODevice::ReadOnly);
        if(isok == true)
        {
            QDataStream stream(&file);
            QFileInfo fileInfo(file);
            int FileSize;
            FileSize = fileInfo.size();
            m = new char[FileSize];
            stream.readRawData(m, FileSize);   // readRawData  读取原始的二进制格式
            short* res = new short[FileSize / sizeof(short)];

            memset(res, 0, FileSize / sizeof(short));

            memcpy(res, m, FileSize);
            quint16Data = res;
            quint16DataLen = FileSize / sizeof(short);
            qDebug() << "数据前三个数：" << quint16Data[0] << quint16Data[1] << quint16Data[2];
            qDebug() << "数据长度：" << quint16DataLen;
            delete [] res;
            res = NULL;
        }
        file.close();
    }
}
