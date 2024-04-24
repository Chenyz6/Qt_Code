#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer timer;
    QChart *chart;              // QT图对象
    QChartView *chartView;      // 创建 QChartView 对象，将 QChart 对象传递给 QChartView 对象进行显示
    QVBoxLayout *layout;        // 矢量分析图控件显示窗口对象
    QLineSeries *series;
};
#endif // MAINWINDOW_H
