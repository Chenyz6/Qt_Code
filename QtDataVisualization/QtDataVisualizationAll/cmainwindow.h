#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QAbstractButton>
#pragma execution_character_set("utf-8")
// 导入数据可视化头文件
#include <QtDataVisualization>
//! 和QChart一样是要使用对应的命名空间
using namespace QtDataVisualization;

namespace Ui {
class CMainWindow;
}
//! 设置一个模板函数
template<class T>
void setSeriesStyle(T graphi, int index);

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

    /**
     * @brief create3DBarGraph 创建三维柱状图
     * @return 返回三维柱状图指针
     */
    QAbstract3DGraph * create3DBarGraph();

    /**
     * @brief create3DScatterGraph 创建三维散点图
     * @return 返回散点图指针
     */
    QAbstract3DGraph * create3DScatterGraph();

    /**
     * @brief create3DSurfaceGraph 创建三维曲面图
     * @return 返回三维曲面图指针
     */
    QAbstract3DGraph * create3DSurfaceGraph();

    /**
     * @brief createValue3DAxis 创建值坐标轴
     * @param axisTitle 坐标轴标题
     * @param titleVisible 是否显示标题
     * @param min 坐标轴最小值
     * @param max 坐标轴最大值
     * @return 返回值坐标轴指针
     */
    QValue3DAxis *createValue3DAxis(QString axisTitle, bool titleVisible = true, float min = 0, float max = 100);

    /**
     * @brief createCategory3DAxis 创建文本坐标轴
     * @param axisTitle 坐标轴标题
     * @param titleVisible 是否显示坐标轴
     * @param labList 坐标轴标签
     * @return 返回值坐标轴指针
     */
    QCategory3DAxis *createCategory3DAxis(QString axisTitle, bool titleVisible = true, QStringList labList = QStringList());

    // QObject interface
protected:
    /**
     * @brief timerEvent 定时器时间
     * @param event 定时器对象
     */
    void timerEvent(QTimerEvent *event);

private slots:
    /**
     * @brief on_angleValueChange 视角改变槽函数
     * @param val 角度值
     */
    void on_angleValueChange(int val);

    /**
     * @brief on_otherOptionGroup_buttonClicked 其他选项按钮组槽函数
     * @param button 点击的按钮指针
     */
    void on_otherOptionGroup_buttonClicked(QAbstractButton *button);

    /**
     * @brief on_seriesStyleComboBox_currentIndexChanged 系列样式设置槽函数
     * @param index 样式索引值
     */
    void on_seriesStyleComboBox_currentIndexChanged(int index);

    /**
     * @brief on_themeComboBox_currentIndexChanged 主题选择槽函数
     * @param index 主题索引值
     */
    void on_themeComboBox_currentIndexChanged(int index);

    /**
     * @brief on_selectModeComboBox_currentIndexChanged 选择模式槽函数
     * @param index 选择模式索引值
     */
    void on_selectModeComboBox_currentIndexChanged(int index);

    /**
     * @brief on_scaleSlider_sliderMoved 缩放功能槽函数
     * @param position 缩放值
     */
    void on_scaleSlider_sliderMoved(int position);

    /**
     * @brief on_autoSwitchAngleBtn_clicked 自动切换视角按钮槽函数
     * @param checked 按钮选中状态
     */
    void on_autoSwitchAngleBtn_clicked(bool checked);

    /**
     * @brief on_typeComboBox_currentIndexChanged 图表类型选选择槽函数
     * @param index 类型索引值
     */
    void on_typeComboBox_currentIndexChanged(int index);

private:
    Ui::CMainWindow             *ui;            // ui对象指针

    QList<QAbstract3DGraph *>   m_graphLsit;    // 图表容器指针

    int                         m_timer;        // 定时器对象

};

#endif // CMAINWINDOW_H

