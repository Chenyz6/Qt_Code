#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QTextStream>
#include <QThread>
#include <QDebug>
#include <QProgressBar>
#include "textedit.h"
#include "global.h"
#include "Common_Inner.h"

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

    void UpdateTime();                  // 状态栏刷新显示时间
    void CreateChildWidget();           // 创建子窗口并显示
    void SetTabbedView();               // MDI模式显示
    void InitTimer();                   // 初始化定时器
    void InitPara();                    // 初始化参数3
    void InitProgress();                // 初始化状态栏进度条

public slots:
    void on_pushButton_framestructuredetection_clicked(); // 启动帧结构检测
    void on_pushButton_BCH_clicked();               // 启动BCH编码
    void on_pushButton_RS_clicked();                // 启动RS编码
    void on_pushButton_CONV_clicked();              // 启动CONV编码
    void on_pushButton_DESSR_clicked();             // 启动DESSR编码
    void on_pushButton_TPC_clicked();               // 启动TPC
    void on_pushButton_ldpc_clicked();              // 启动LDPC
    void on_pushButton_opencodingidentify_clicked();// 启动编码识别
    void on_pushButton_codinglastpage_clicked();    // 上一页
    void on_pushButton_codingnextpage_clicked();    // 下一页
    void on_action_Open_triggered();                // 工具栏点击打开文件
    void ReceiveThreadFinish(short index);            // 接收到线程处理完毕

private:
    Ui::MainWindow *ui;
    TextEdit * textEdit;                // 创建文件编辑对象
    QLabel*	LTime = new QLabel;			// 时间信息标签
    QTimer*	timer = new QTimer;         // 状态栏时间定时器
    QTimer*	progressBarTimer = new QTimer;  // 状态栏进度条定时器
    QProgressBar* progressBar = new QProgressBar;       // 状态栏进度条对象
};
#endif // MAINWINDOW_H
