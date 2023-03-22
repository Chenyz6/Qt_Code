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

}
