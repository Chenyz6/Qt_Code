#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerWidget; }
QT_END_NAMESPACE

class ServerWidget : public QWidget
{
    Q_OBJECT

public:
    ServerWidget(QWidget *parent = nullptr);
    ~ServerWidget();

    void sendData(); // 发送文件数据

private slots:
    void on_pushButtonFile_clicked();

    void on_pushButtonSend_clicked();

private:
    Ui::ServerWidget *ui;

    QTcpServer *tcpserver;
    QTcpSocket *tcpsocket;

    QFile file; // 文件对象
    QString fileName; // 文件名字
    qint64 fileSize; // 文件大小
    qint64 sendSize; // 已经发送文件大小
    QTimer timer;
};
#endif // SERVERWIDGET_H
