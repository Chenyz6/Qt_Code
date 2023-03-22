#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "qcustomplot.h"
#pragma execution_character_set("utf-8")
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void drawImage(); //画图
    void contextMenuEvent(QContextMenuEvent *event); //第一种方式改主函数右键菜单选项
    void on_tableViewCustomContextMenuRequested(const QPoint &pos);  //第二种方式改控件的右键菜单选项
    void ContextMenuRequested(const QPoint &pos);
    void customPlot_init(); //初始化频谱图
    void InitWaterfall(); //初始化瀑布图
    void customPlot_show();   //显示频谱图


private:
    Ui::Widget *ui;
    QCPGraph *pGraph;							//向绘图区域QCustomPlot添加一条曲线
    QCPGraph *pGraphMax;						//峰值保持
    QCPColorMap* m_pColorMap;					//绘制色谱图指针变量
    QCPColorScale* m_pColorScale;				//瀑布图色条
};
#endif // WIDGET_H
