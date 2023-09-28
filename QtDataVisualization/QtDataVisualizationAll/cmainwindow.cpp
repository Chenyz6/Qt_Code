#include "CMainWindow.h"
#include "ui_CMainWindow.h"
#include <QtDebug>

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow)
{
    ui->setupUi(this);
    // 设置标题
    this->setWindowTitle("三维图形的简单使用");

    // 创建三维柱状图
    m_graphLsit.append(create3DBarGraph());
    // 创建三维散点图
    m_graphLsit.append(create3DScatterGraph());
    // 创建三维曲面图
    m_graphLsit.append(create3DSurfaceGraph());

    // 重置随机数种子
    srand(QDateTime::currentSecsSinceEpoch() % 1000000);

//    ui->otherOptionGroup->setExclusive(false);  // 设置按钮组可多选(ui界面已设置)
}

CMainWindow::~CMainWindow()
{
    // 遍历删除图表对象
    for(int index = m_graphLsit.count() - 1; index != -1; --index)
    {
        // 释放图表
        delete m_graphLsit.at(index);
    }
    // 删除ui对象
    delete ui;
}

QAbstract3DGraph *CMainWindow::create3DBarGraph()
{
    // 创建三维柱状图对象
    Q3DBars *bars = new Q3DBars;
    // 创建三维柱状图容器
    QWidget *container = QWidget::createWindowContainer(bars);
    // 将当前容器添加到栈窗口对象中，并设置容器为栈窗口当前窗口
    ui->stackedWidget->addWidget(container);
    ui->stackedWidget->setCurrentWidget(container);


    // 创建三维柱状图坐标轴对象
    bars->setValueAxis(createValue3DAxis("Value Axis", true, 0, 10));
    // 创建坐标轴标签容器并添加到坐标轴中
    // 列坐标轴
    QStringList colLabs;
    colLabs << "Column1" << "Column2" << "Column3";
    bars->setColumnAxis(createCategory3DAxis("Column Category Axis", true, colLabs));
    // 行坐标轴
    QStringList rowLabs;
    rowLabs << "Row1" << "Row2" << "Row3";
    bars->setRowAxis(createCategory3DAxis("Row Category Axis", true, rowLabs));


    // 创建三维柱状图系列对象
    QBar3DSeries *series = new QBar3DSeries;
    // 将系列对象添加到三维柱状图对象中
    bars->addSeries(series);

    // 创建三维柱状图数据容器
    QBarDataArray *array = new QBarDataArray;
    // 循环创建数据
    for(int index = 0; index != 3; ++index)
    {
        // 创建柱状图行数据容器
        QBarDataRow *dataRow = new QBarDataRow;
        // 使用随机数添加数据
        *dataRow << rand() % 10 << rand() % 10 << rand() % 10 << rand() % 10 << rand() % 10;
        // 将行数据添加到array对象中
        array->append(dataRow);
    }
    // 将创建的数据添加到系列对象中(当指针容器指针不同时，将重置添加)
    series->dataProxy()->resetArray(array);

    // 设置动态属性（类型，作用为在设置系列样式槽函数中区分）
    bars->setProperty("Type", 0);

    // 返回三维柱状图对象
    return bars;
}

QAbstract3DGraph *CMainWindow::create3DScatterGraph()
{
    // 创建三维散点图对象
    Q3DScatter *scatter = new Q3DScatter;
    // 创建三维散点图容器
    QWidget *container = QWidget::createWindowContainer(scatter);
    // 将当前容器添加到栈窗口对象中
    ui->stackedWidget->addWidget(container);

    //! 创建三维散点图的坐标轴
    //! 因为是三维散点图，所以包括X、Y、Z三个方向的坐标轴(并且三个坐标轴类型都为值坐标轴哦)
    // 创建X、Y、Z轴并添加
    scatter->setAxisX(createValue3DAxis("X Axis"));
    scatter->setAxisY(createValue3DAxis("Y Axis"));
    scatter->setAxisZ(createValue3DAxis("Z Axis"));


    // 创建三维散点图的系列对象
    QScatter3DSeries *series = new QScatter3DSeries;
    // 将系列对象添加到三维散点图对象中
    scatter->addSeries(series);

    // 创建三维散点图的数据容器
    QScatterDataArray array;
    // 循环添加数据
    for(int index = 0; index != 30; ++index)
    {
        // 使用随机数添加点
        array.append(QVector3D(rand() % 100, rand() % 100, rand() % 100));
    }
    // 将创建的数据添加到系列对象中(做追加数组的操作)
    series->dataProxy()->addItems(array);

    // 设置动态属性（类型，作用为在设置系列样式槽函数中区分）
    scatter->setProperty("Type", 1);

    // 返回三维散点图指针
    return scatter;
}

QAbstract3DGraph *CMainWindow::create3DSurfaceGraph()
{
    // 创建三维曲面图对象
    Q3DSurface *surface = new Q3DSurface;
    // 创建三维曲面图容器
    QWidget *container = QWidget::createWindowContainer(surface);
    // 将容器添加到栈窗口中
    ui->stackedWidget->addWidget(container);


    //! 创建三维散点图的坐标轴
    //! 因为是三维散点图，所以包括X、Y、Z三个方向的坐标轴(并且三个坐标轴类型都为值坐标轴哦)
    // 创建X、Y、Z轴并添加
    surface->setAxisX(createValue3DAxis("X Axis"));
    surface->setAxisY(createValue3DAxis("Y Axis"));
    surface->setAxisZ(createValue3DAxis("Z Axis"));


    // 创建三维曲面图系列对象
    QSurface3DSeries *series = new QSurface3DSeries;
    // 将系列添加到三维曲面图中
    surface->addSeries(series);

    // 创建三维曲面图数据容器
    QSurfaceDataArray *array = new QSurfaceDataArray;
    // 创建三维曲面图数据
    for(int index = 0; index != 5; ++index)
    {
        // 创建三维曲面图行数据容器
        QSurfaceDataRow *dataRow = new QSurfaceDataRow;
        // 遍历添加数据到行容器
        for(int valIdx = 0; valIdx != 3; ++valIdx)
        {
            // 随机数添加数据
            dataRow->append(QVector3D(rand() % 100, rand() % 100, rand() % 100));
        }
        // 将行容器添加到array中
        array->append(dataRow);
    }
    // 将数据添加到系列对象中
    series->dataProxy()->resetArray(array);

    //! 因为曲面图是面，所以切换系列类型没有变化
    //! 若要设置面的样式可像下方一样调用setDrawMode函数
    //! surface->seriesList()[0]->setDrawMode(QSurface3DSeries::DrawFlag(vlaue));
    //! value:枚举值 其中枚举值如下
    //! QSurface3DSeries::DrawWireframe 仅绘制网格。
    //! QSurface3DSeries::DrawSurface   仅绘制曲面。
    //! QSurface3DSeries::DrawSurfaceAndWireframe    绘制曲面和栅格。

    //! 设置动态属性（类型，作用为在设置系列样式槽函数中区分）
    surface->setProperty("Type", -1);


    // 返回三维曲面图对象
    return surface;

}

QValue3DAxis *CMainWindow::createValue3DAxis(QString axisTitle, bool titleVisible, float min, float max)
{
    // 创建值坐标轴对象
    QValue3DAxis *axis = new QValue3DAxis;
    axis->setTitle(axisTitle);  // 设置坐标轴标题
    axis->setTitleVisible(titleVisible); // 设置标题是否显示
    axis->setRange(min, max);   // 设置坐标轴取值范围
    // 返回坐标轴对象
    return axis;
}

QCategory3DAxis *CMainWindow::createCategory3DAxis(QString axisTitle, bool titleVisible, QStringList labList)
{
    // 创建文本坐标轴对象
    QCategory3DAxis *axis = new QCategory3DAxis;
    axis->setTitle(axisTitle);  // 设置坐标轴标题
    axis->setTitleVisible(titleVisible); // 设置标题是否显示
    axis->setLabels(labList);   // 设置坐标轴标签
    // 返回坐标轴对象
    return axis;

}

void CMainWindow::on_angleValueChange(int val)
{
    // 拿到信号发送者
    QSpinBox *spinBox = dynamic_cast<QSpinBox *>(sender());

    // 若当前为定时改变视角则不设置活动摄像角度，并将动态属性值设置为false
    if(spinBox->property("RotationFlag").toBool())
    {
        spinBox->setProperty("RotationFlag", false);
        return;
    }

    // 拿到视角类型
    int type = spinBox->property("DirectionType").toInt();

    // 判断当前方向类型，并将角度赋到对应位置
    if(0 == type)
    {
        // 遍历设置所有三维图的X轴视角
        foreach(QAbstract3DGraph *graph, m_graphLsit)
        {
            // 获取图表的视图->活动摄像头->设置角度
            graph->scene()->activeCamera()->setXRotation(val);
        }
    }
    else if(1 == type)
    {
        // 遍历设置所有三维图的Y轴视角
        foreach(QAbstract3DGraph *graph, m_graphLsit)
        {
            // 获取图表的视图->活动摄像头->设置角度
            graph->scene()->activeCamera()->setYRotation(val);
        }
    }

}

void CMainWindow::on_otherOptionGroup_buttonClicked(QAbstractButton *button)
{
    // 将按钮对象转为复选框对象
    QCheckBox *curBox = dynamic_cast<QCheckBox *>(button);
    // 获取当前按钮的类型
    int type = curBox->property("OptionType").toInt();
    // 获取按钮选择状态
    bool checkedStatus = curBox->isChecked();
    switch (type)
    {
    case 0:
    {
        // 循环设置图表状态
        foreach(QAbstract3DGraph *graph, m_graphLsit)
        {
            // 设置背景可用
            graph->activeTheme()->setBackgroundEnabled(checkedStatus);
            // 标签可用
            graph->activeTheme()->setLabelBackgroundEnabled(checkedStatus);
        }
        break;
    }
    case 1:
    {
        // 循环设置图表状态
        foreach(QAbstract3DGraph *graph, m_graphLsit)
        {
            // 设置显示网格
            graph->activeTheme()->setGridEnabled(checkedStatus);
        }
        break;
    }
    case 2:
    {
        // 循环设置图表状态
        foreach(QAbstract3DGraph *graph, m_graphLsit)
        {
            // 设置显示倒影
            graph->setReflection(checkedStatus);
        }
        break;
    }
    case 3:
    {
        // 循环设置图表状态
        foreach(QAbstract3DGraph *graph, m_graphLsit)
        {
            // 设置正交投影显示（偏二维）
            graph->setOrthoProjection(checkedStatus);
        }
        break;
    }
    default:
        break;
    }
}

void CMainWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    // 创建当前摄像头对象
    Q3DCamera *curCamera;
    // 循环设置相机视角
    foreach(QAbstract3DGraph *graph, m_graphLsit)
    {
        // 获取当前视图视角
        curCamera = graph->scene()->activeCamera();
        // 获取当前视角值
        int curCameraPreset = curCamera->cameraPreset();
        // 判断获取下一是视角值
        int cameraPreset = curCameraPreset == 21? 1: curCameraPreset + 1;
        // 设置相机视角
        curCamera->setCameraPreset(Q3DCamera::CameraPreset(cameraPreset));
    }

    // 设置水平视角控件值
    ui->horAngleSpinBox->setValue(curCamera->xRotation());
    // 设置水平动态属性
    ui->horAngleSpinBox->setProperty("RotationFlag", true);
    // 设置垂直视角控件值
    ui->verAngleSpinBox->setValue(curCamera->yRotation());
    // 设置水平动态属性
    ui->verAngleSpinBox->setProperty("RotationFlag", true);
}

void CMainWindow::on_seriesStyleComboBox_currentIndexChanged(int index)
{
    // 循环设置图表系列状态
    foreach(QAbstract3DGraph *graph, m_graphLsit)
    {
        int type = graph->property("Type").toInt();
        // 获取当前图表类型
        switch (type)
        {
        case 0:
        {
            // 调用样式模板函数
            setSeriesStyle(dynamic_cast<Q3DBars *>(graph), index);
            break;
        }
        case 1:
        {
            // 调用样式模板函数
            setSeriesStyle(dynamic_cast<Q3DScatter *>(graph), index);
            break;
        }
        default:
            break;
        }
    }
}

void CMainWindow::on_themeComboBox_currentIndexChanged(int index)
{
    // 循环设置图表主题
    foreach(QAbstract3DGraph *graph, m_graphLsit)
    {
        graph->activeTheme()->setType(Q3DTheme::Theme(index));
    }
}

void CMainWindow::on_selectModeComboBox_currentIndexChanged(int index)
{
    // 设置柱状图的选择模式
    m_graphLsit.first()->setSelectionMode(QAbstract3DGraph::SelectionFlag(index));
}

void CMainWindow::on_scaleSlider_sliderMoved(int position)
{
    // 循环设置图表缩放
    foreach(QAbstract3DGraph *graph, m_graphLsit)
    {
        graph->scene()->activeCamera()->setZoomLevel(position);
    }
}

void CMainWindow::on_autoSwitchAngleBtn_clicked(bool checked)
{
    // 根据状态做出相应操作
    if(checked)
    {
        // 改变按钮文本
        ui->autoSwitchAngleBtn->setText("停止");
        // 启动定时器
        m_timer = startTimer(750);
    }
    else
    {
        // 改变按钮文本
        ui->autoSwitchAngleBtn->setText("开始");
        // 终止定时器
        killTimer(m_timer);
    }
}

template<class T>
void setSeriesStyle(T graphi, int index)
{
    // 循环设置图表样式
    foreach(QAbstract3DSeries *series, graphi->seriesList())
    {
        //! 设置样式
        //! 索引值加1是防止设置值为0的Mesh，未作出对应操作设置该值的样式会导致程序崩溃
        //! 帮助中这样形容（翻译）：用户定义网格，通过QAbstract3DSeries:：userDefinedMesh属性设置。
        series->setMesh(QAbstract3DSeries::Mesh(index + 1));
    }
}

void CMainWindow::on_typeComboBox_currentIndexChanged(int index)
{
    // 设置当前显示的图表
    ui->stackedWidget->setCurrentIndex(index);

    //! 判断选择模式禁用，仅在三维柱状图下可用
    //! 因为在测试时发现本文中的三维散点图仅支持“无”和单项选择模式，三维曲面图不支持选择模式
    ui->selectModeComboBox->setEnabled(0 == index);

    //! 判断显示倒影禁用，仅在三维柱状图下可用
    //! 因为在测试时发现本文中的三维散点图、三维曲面图并无倒影显示
    ui->showReflectionCheckBox->setEnabled(0 == index);

    //! 判断设置系列样式禁用
    //! 三维曲面图设置Mesh无效，则禁用
    ui->seriesStyleComboBox->setEnabled(2 != index);

}

