#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QVector>
#include <QDataStream>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>
#include <QFile>
#include <QFileDialog>
#pragma execution_character_set("utf-8")
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool is_first_connect(QLocalSocket *newsocket);

private slots:
    void on_pushButton_clicked();
    void new_connection();
    void recv_data();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    QLocalServer *m_server;

    QList<QLocalSocket *> local_sockets;  //利用Qlist来存储新连接


    short*  quint16Data;
    int quint16DataLen;
};
#endif // MAINWINDOW_H
