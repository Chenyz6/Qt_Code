#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QFile>
#include <QMessageBox>
#include <QHostAddress>

namespace Ui {
class ClientWidget;
}

class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientWidget(QWidget *parent = nullptr);
    ~ClientWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ClientWidget *ui;
    QTcpSocket * tcpsocket;

    QFile file; // 文件对象
    QString fileName; // 文件名字
    qint64 fileSize; // 文件大小
    qint64 recvSize; // 已经发送文件大小

    bool isStart;
};

#endif // CLIENTWIDGET_H
