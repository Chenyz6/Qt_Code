#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>
#include <QTcpServer>  //监听套接字
#include <QTcpSocket>  //通信套接字
#pragma execution_character_set("utf-8")
QT_BEGIN_NAMESPACE
namespace Ui { class serverwidget; }
QT_END_NAMESPACE

class serverwidget : public QWidget
{
    Q_OBJECT

public:
    serverwidget(QWidget *parent = nullptr);
    ~serverwidget();

private slots:
    void on_pushButtonSend_clicked();

    void on_pushButton_clicked();

private:
    Ui::serverwidget *ui;
    QTcpServer * tcpserver;
    QTcpSocket * tcpsocket;
};
#endif // SERVERWIDGET_H
