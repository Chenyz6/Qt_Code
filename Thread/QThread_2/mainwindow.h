#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QThread>
#include <QMainWindow>
#include "mythread.h"
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

private slots:
    void on_start_clicked();
signals:
    void starting(int num);

private:
    Ui::MainWindow *ui;

    // 1.创建子线程对象
    QThread * t1 = new QThread(this);
    QThread * t2 = new QThread(this);

    // 2.创建任务类的对象
    Generate * gen = new Generate;   // 不能加this
    BubbleSort * bubble = new BubbleSort;
};
#endif // MAINWINDOW_H
