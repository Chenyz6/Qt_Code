#include "serverwidget.h"
#include "ui_serverwidget.h"

serverwidget::serverwidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::serverwidget)
{
    ui->setupUi(this);

    tcpserver = NULL;
    tcpsocket = NULL;
    resize(400,300);
    setWindowTitle("server：8888");
    tcpserver = new QTcpServer(this); //制定父对象，让其自动回收空间
    tcpserver->listen(QHostAddress::Any, 8888);
    connect(tcpserver, &QTcpServer::newConnection,
            [=]()
            {
                //取出建立好连接的套接字
                tcpsocket = tcpserver->nextPendingConnection();//pending即将发生的
                //取出对方的ip,端口地址
                QString ip = tcpsocket->peerAddress().toString();
                qint16 port = tcpsocket->peerPort();
                QString temp = QString("[%1 : %2] 成功连接").arg(ip),arg(port);
                ui->textEditRead->setText(temp);

                connect(tcpsocket, &QTcpSocket::readyRead,  // 有数据进来准备读
                        [=]()
                        {
                            QByteArray array = tcpsocket->readAll();
                            //QByteArray  readall返回的是字符数组
                            ui->textEditRead->append(array);  // 追加内容
                        }
                        );
            }
            );
}

serverwidget::~serverwidget()
{
    delete ui;
}


void serverwidget::on_pushButtonSend_clicked()
{
    if(NULL == tcpsocket)
    {
        return;
    }
    //获取编辑区的内容
    QString text = ui->textEditWrite->toPlainText();
    //简单文本用toPlainText()
    //QLineEdit(行编辑时) 时用text()
    //发送数据
    tcpsocket->write(text.toUtf8().data());  //data()  取出数据
}

void serverwidget::on_pushButton_clicked()
{
    if(NULL == tcpsocket)
    {
        return;
    }
    //主动和客户端断开连接
    tcpsocket->disconnectFromHost();
    tcpsocket->close();
    tcpsocket = NULL;
}
