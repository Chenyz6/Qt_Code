#include "widget.h"
#include "ui_widget.h"
#include <QHostAddress>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 分配空间
    udpsocket = new QUdpSocket(this);
    // 绑定
    udpsocket->bind(8888);
    setWindowTitle("服务器端口：8888");
    connect(udpsocket,&QUdpSocket::readyRead,
            [=]()
            {
                char buf[1024] = {0};
                QHostAddress clientAddr; // 对方地址
                quint16 port; // 对方端口
                qint64 len = udpsocket->readDatagram(buf,sizeof(buf),&clientAddr,&port);
                if(len > 0)
                {
                    QString str = QString("[%1:%2]%3")
                            .arg(clientAddr.toString()
                            .arg(port)
                            .arg(buf));
                    ui->textEdit->setText(str);
                }
            });
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButtonSend_clicked()
{
    // 获取对方IP port
    QString ip = ui->lineEditIP->text();
    qint16 port = ui->lineEditPort->text().toInt();
    // 获取编辑区内容
    QString string = ui->textEdit->toPlainText();
    // 发送
    udpsocket->writeDatagram(string.toUtf8(),QHostAddress(ip),port);
}
