#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QProcess>
#include <QVector>
#include <QDataStream>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>
#include <QTimer>
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

    void rcv_data();
    void connect_success();
    void disconnect_from_server();
    void reconect_to_server();
    void error_proc(QLocalSocket::LocalSocketError state);

private:
    Ui::MainWindow *ui;

    QLocalSocket * m_socket = new QLocalSocket;
    QTimer * reconnect_timer = new QTimer;
};
#endif // MAINWINDOW_H
