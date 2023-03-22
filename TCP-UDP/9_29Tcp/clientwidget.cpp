#include "clientwidget.h"
#include "ui_clientwidget.h"
#include <QHostAddress>
clientwidget::clientwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::clientwidget)
{
    ui->setupUi(this);
    setWindowTitle("client");
    resize(400,300);
    tcpsocket = NULL;
    tcpsocket = new QTcpSocket(this);//制定父对象 让其自动回收空间
    connect(tcpsocket,&QTcpSocket::connected,
            [=]()
            {
                ui->textEditRead->setText("已经成功和服务器连接");
            });
    connect(tcpsocket,&QTcpSocket::readyRead,
            [=]()
            {
                // 获取对方发送的内容
                QByteArray array = tcpsocket->readAll();
                // 追加到编辑区
                ui->textEditRead->append(array);
            });
}

clientwidget::~clientwidget()
{
    delete ui;
}

void clientwidget::on_pushButtonConnect_clicked()
{
    //获取服务器端口和ip
    qint16 port = ui->lineEditPort->text().toInt();
    QString ip = ui->lineEditIp->text();
    //与服务期进行连接
    tcpsocket->connectToHost(QHostAddress(ip), port); //  直接ip也可以
}

void clientwidget::on_pushButtonSend_clicked()
{
    // 获取编辑框内容
    QString string = ui->textEditWrite->toPlainText();
    // 发送数据
    tcpsocket->write(string.toUtf8().data());
}



void clientwidget::on_pushButtonClose_clicked()
{
    // 主动和对方断开连接
    tcpsocket->disconnectFromHost();
    tcpsocket->close();
}
