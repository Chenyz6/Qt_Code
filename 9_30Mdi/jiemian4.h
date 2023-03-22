#ifndef JIEMIAN4_H
#define JIEMIAN4_H

#include <QMainWindow>
#include <QLabel>
#include "qcustomplot.h"
#pragma execution_character_set("utf-8")
namespace Ui {
class jiemian4;
}

class jiemian4 : public QMainWindow
{
    Q_OBJECT

public:
    explicit jiemian4(QWidget *parent = nullptr);
    ~jiemian4();
    void StatusBarTime();
    void InitSpectrum();  //初始化频谱图
                    //向绘图区域QCustomPlot添加一条曲线
private slots:
    void on_pushButton_clicked();

private:
    Ui::jiemian4 *ui;
    QTimer* timer;						//时间显示定时器
    QLabel*	LabelTime = new QLabel;
    QCPGraph *pGraph;					//向绘图区域QCustomPlot添加一条曲线
};

#endif // JIEMIAN4_H
