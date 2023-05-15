#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "qcustomplot.h"
#include <QMainWindow>
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
    void init_Constellation();   // 初始化星座图
    void init_Waterfall();       // 初始化瀑布图
    void init_DirectionFind();	 //初始化测向图
    bool eventFilter(QObject *watched, QEvent *event);			//事件过滤器
    void updata();

private:
    Ui::MainWindow *ui;

    QCPColorMap* m_pColorMap;					//绘制色谱图指针变量
    QCPColorScale* m_pColorScale;				//瀑布图色条
    QTimer *time = new QTimer;
    QTimer *time1 = new QTimer;
    QTimer *time2 = new QTimer;

    double nAzimuth = 0;		//方位角
    int q = 1;
};
#endif // MAINWINDOW_H
