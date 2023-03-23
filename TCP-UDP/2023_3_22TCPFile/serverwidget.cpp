#include "serverwidget.h"
#include "ui_serverwidget.h"


ServerWidget::ServerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerWidget)
{
    ui->setupUi(this);
    setWindowTitle("Server  Port:8888");

    ui->pushButtonFile->setEnabled(false);
    ui->pushButtonSend->setEnabled(false);

    tcpserver = new QTcpServer(this);
    tcpserver->listen(QHostAddress::Any,8888);
    connect(tcpserver,&QTcpServer::newConnection,
            [=]()
            {
                tcpsocket = tcpserver->nextPendingConnection();
                QString ip = tcpsocket->peerAddress().toString();
                quint64 port = tcpsocket->peerPort();

                QString string = QString("[%1,%2] Successful connection").arg(ip).arg(port);
                ui->textEdit->setText(string);

                ui->pushButtonFile->setEnabled(true);
                ui->pushButtonSend->setEnabled(true);
            });
    connect(&timer,&QTimer::timeout,
            [=]()
            {
                // 关闭定时器
                timer.stop();
                sendData();
            });
}

ServerWidget::~ServerWidget()
{
    delete ui;
}

void ServerWidget::on_pushButtonFile_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,"Open","../");
    if(false == filepath.isEmpty()) // 如果选择文件路径有效
    {
        fileName.clear();
        fileSize = 0;
        // 获取文件信息
        QFileInfo info(filepath);
        fileName = info.fileName();
        fileSize = info.size();
        sendSize = 0;
        // 只读方式打开
        file.setFileName(filepath);
        // 打开文件
        bool isOk = file.open(QIODevice::ReadOnly);
        if(false == isOk)
        {
            qDebug() << "readonly file open error";
        }
        ui->textEdit->append(filepath);
        ui->pushButtonFile->setEnabled(false);
        ui->pushButtonFile->setEnabled(true);
    }
    else
    {
        qDebug() << "Choose file error";
    }
}


void ServerWidget::on_pushButtonSend_clicked()
{
    // 头部信息
    QString head = QString("%1##%2").arg(fileName).arg(fileSize);
    // 发送头部信息
    qint64 len = tcpsocket->write(head.toUtf8());
    if(len > 0)
    {
        // 发送文件信息
        // 防止TCP粘包问题
        // 添加定时器
        timer.start(20);
    }
    else
    {
        qDebug() << "head send error";
        file.close();
        ui->pushButtonFile->setEnabled(true);
        ui->pushButtonSend->setEnabled(false);
    }
}

void ServerWidget::sendData()
{
    qint64 len = 0;
    do
    {
        char buf[4*1024] = {0}; // 每次发送的大小
        len = 0;
        len = file.read(buf,sizeof(buf)); // 文件中读数据
        len = tcpsocket->write(buf,len);
        sendSize += len;
    }while(len > 0);

    if(sendSize == fileSize)
    {
        ui->textEdit->append("file send finish");
        file.close();

        tcpsocket->disconnectFromHost();
        tcpsocket->close();
    }
}
