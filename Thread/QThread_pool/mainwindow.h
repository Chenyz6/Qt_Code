#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mythread.h"
#include <QDebug>
#pragma execution_character_set("utf-8")
#define cout qDebug() <<"["<<__FILE__<<":"<<__LINE__<<"]"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_start_clicked();
signals:
    void starting(int num);

private:
    Ui::MainWindow *ui;
    Generate * gen = new Generate;
    BubbleSort * bubble = new BubbleSort;
};
#endif // MAINWINDOW_H
