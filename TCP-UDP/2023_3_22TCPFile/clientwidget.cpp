#include "clientwidget.h"
#include "ui_clientwidget.h"

ClientWidget::ClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);
    tcpsocket = new QTcpSocket(this);
    isStart = true;
    connect(tcpsocket,&QTcpSocket::readyRead,
            [=]()
            {
                QByteArray buf = tcpsocket->readAll();
                if(true == isStart)
                {   // 接受头
                    isStart = false;
                    // 解析头部信息
                    // 初始化
                    fileName = QString(buf).section("##", 0, 0);
                    fileSize = QString(buf).section("##", 1, 1).toInt();
                    recvSize = 0;
                    // 打开文件
                    file.setFileName(fileName);
                    bool isOK = file.open(QIODevice::WriteOnly);
                    if(false == isOK)
                    {
                        qDebug() << "WriteOnly error";
                    }
                }
                else // 文件信息
                {
                    qint64 len = file.write(buf);
                    recvSize += len;
                    if(recvSize == fileSize)
                    {
                        file.close();
                        QMessageBox::information(this,"finish","file finish");
                        tcpsocket->disconnectFromHost();
                        tcpsocket->close();
                    }
                }
            });
}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::on_pushButton_clicked()
{
    QString ip = ui->lineEditIP->text();
    quint16 port = ui->lineEditPort->text().toInt();

    tcpsocket->connectToHost(QHostAddress(ip),port);
}
