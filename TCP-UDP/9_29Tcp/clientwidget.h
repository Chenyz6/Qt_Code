#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>
#include <QTcpSocket>

#pragma execution_character_set("utf-8")
namespace Ui {
class clientwidget;
}

class clientwidget : public QWidget
{
    Q_OBJECT

public:
    explicit clientwidget(QWidget *parent = nullptr);
    ~clientwidget();

private slots:
    void on_pushButtonConnect_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonClose_clicked();

private:
    Ui::clientwidget *ui;
    QTcpSocket * tcpsocket;
};

#endif // CLIENTWIDGET_H
