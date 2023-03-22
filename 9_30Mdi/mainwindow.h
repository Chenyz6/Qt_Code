#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma execution_character_set("utf-8")
#include <QMainWindow>
#include <add1.h>
#include <add2.h>
#include <add3.h>
#include <jiemian4.h>
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
    void on_actionTab_triggered();

    void on_action_9_triggered();

    void on_action_5_triggered();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    add1 jiemian1;
    add2 jiemian2;
    add3 jiemian3;
    jiemian4 jiemian4;
};
#endif // MAINWINDOW_H
