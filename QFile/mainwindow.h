#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Filecontrol.h"
#include "mythread.h"
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);

    double* CalSingalFrequency(double* pData, long nDataLen);
    ~MainWindow();
public slots:
    void on_pushButton_clicked();
    void deal();
private slots:
    void on_pushButton_2_clicked();
    void DealDone();
    void stopThread();
private:
    Ui::MainWindow *ui;
    MyThread * thread;

};
#endif // MAINWINDOW_H
