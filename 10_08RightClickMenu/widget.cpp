#include "widget.h"
#include "ui_widget.h"
#include "qcustomplot.h"
#include <QMenu>
#include <QStandardItemModel>
#include <QDebug>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    drawImage();
    //调用setContextMenuPolicy并设置其值（如果不添加这行，对应的控件则不会发出对应的信号）
    ui->widget->setContextMenuPolicy(Qt::CustomContextMenu);
    //连接右键菜单对应的信号槽
    connect(ui->widget, &QTableView::customContextMenuRequested
            , this, &Widget::on_tableViewCustomContextMenuRequested);
    connect(ui->qcustomplot, &QTableView::customContextMenuRequested
            , this, &Widget::ContextMenuRequested);
    customPlot_init();

    customPlot_show();

    InitWaterfall();
}



void Widget::InitWaterfall()  //初始化瀑布图
{
    ui->Waterfall->setBackground(QColor(37, 37, 38));	//设置背景颜色(黑)
    ui->Waterfall->xAxis->setVisible(false);				//使x轴网格线不可见
    ui->Waterfall->yAxis->setVisible(false);				//使y轴网格线不可见

    m_pColorMap = new QCPColorMap(ui->Waterfall->xAxis, ui->Waterfall->yAxis);
    //m_pColorMap->data()->setSize(1001, WF_Y_RANGE + 1);									//设置整个图的x,y点数
    //m_pColorMap->data()->setRange(QCPRange(0, 1001), QCPRange(0, WF_Y_RANGE + 1));		//设置x轴和y轴的范围
    m_pColorMap->setDataRange(QCPRange(0, 100));

    m_pColorScale = new QCPColorScale(ui->Waterfall);				//构造一个色条
    m_pColorScale->setType(QCPAxis::atBottom);						//色条数值显示在色条下方
    ui->Waterfall->plotLayout()->addElement(1, 0, m_pColorScale);	//色条显示在瀑布图最下方
    m_pColorScale->setDataRange(QCPRange(0, 100));					//设置色条数值范围
    m_pColorScale->setBarWidth(10);									//设置色条宽度

    QCPColorGradient gradient;										// 色条使用自定义颜色渐变
    gradient.setColorStopAt(0.0, QColor(0, 0, 0));					// 设置色条开始时的颜色(黑
    gradient.setColorStopAt(0.1, QColor(0, 0, 255));				// 蓝
    gradient.setColorStopAt(0.3, QColor(0xcc, 0, 0xff));			// 紫
    gradient.setColorStopAt(0.5, QColor(0xff, 0, 0));				// 红
    gradient.setColorStopAt(0.7, QColor(0xff, 0xff, 0));			// 黄
    gradient.setColorStopAt(0.9, QColor(0, 0xff, 0));				// 绿
    gradient.setColorStopAt(1.0, QColor(255, 255, 255));			// 设置色条结束时的颜色(白

    m_pColorScale->setGradient(gradient);							//将自定义渐变色插入色条
    m_pColorMap->setColorScale(m_pColorScale);						//设置自定义渐近色变化

    ui->Waterfall->yAxis->setRangeReversed(true);					//设置Y轴反向
    m_pColorMap->setInterpolate(true);								//色图平滑变化
    ui->Waterfall->rescaleAxes();									//自适应大小
}



void Widget::customPlot_init()  //初始化频谱图
{
    //添加一条曲线：每一条曲线都必须独占一个graph，新建graph可通过addGraph()来实现
    pGraph = ui->qcustomplot->addGraph();
    // graph(0) 可以获取某个数据曲线（按添加先后排序）
    pGraph->setAdaptiveSampling(true);//启用自适应采样：自适应采样技术可以极大地提高点数较多（例如10,000以上）的图的重绘性能，而无需显着更改图的外观
    pGraph->setPen(QPen(QColor(0, 255, 0)));

    //峰值保持
    pGraphMax = ui->qcustomplot->addGraph();
    pGraphMax->setPen(QPen(QColor(0xFF, 0, 0)));

    ui->qcustomplot->axisRect()->setRangeDrag(Qt::Horizontal);	//设置只能在水平方向进行拖拽
    ui->qcustomplot->axisRect()->setRangeZoom(Qt::Horizontal);	//设置只能在水平方向进行放缩

    ui->qcustomplot->selectionRect()->setPen(QPen(Qt::red, 2, Qt::DashLine));	//设置选框的样式：虚线
    ui->qcustomplot->selectionRect()->setBrush(QBrush(QColor(0, 0, 238, 120)));	//设置选框的样式：半透明浅蓝
    ui->qcustomplot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);		//启动框选放大模式

    // 设置背景颜色（黑）
    ui->qcustomplot->setBackground(QColor(37, 37, 38));

    // 设置x/y轴文本色、轴线色、字体等
    ui->qcustomplot->xAxis->setTickLabelColor(Qt::white);
    ui->qcustomplot->xAxis->setLabelColor(QColor(0, 160, 230));
    ui->qcustomplot->xAxis->setBasePen(QPen(Qt::white, 2));
    ui->qcustomplot->xAxis->setTickPen(QPen(QColor(128, 0, 255)));
    ui->qcustomplot->xAxis->setSubTickPen(QColor(255, 165, 0));

    ui->qcustomplot->yAxis->setTickLabelColor(Qt::white);
    ui->qcustomplot->yAxis->setLabelColor(QColor(0, 160, 230));
    ui->qcustomplot->yAxis->setBasePen(QPen(QColor(32, 178, 170)));
    ui->qcustomplot->yAxis->setTickPen(QPen(QColor(128, 0, 255)));
    ui->qcustomplot->yAxis->setSubTickPen(QColor(255, 165, 0));

}

void Widget::customPlot_show()  //频谱图的显示
{

    QVector<double> xx;
    QVector<double> data;
    ui->qcustomplot->xAxis->setRange(0,1000);
    ui->qcustomplot->yAxis->setRange(-50, 100);
    for(int i=0;i<1000;i++)
    {
        xx <<i;
        data << (qrand() % 100);
    };
    ui->qcustomplot->graph(0)->setData(xx,data);
}

void Widget::ContextMenuRequested(const QPoint & pos)
{
    Q_UNUSED(pos);
    QMenu menu;
    QAction *p1 = menu.addAction("恢复原状");
    connect(p1,&QAction::triggered,
            [=]()
            {
                ui->qcustomplot->graph(0)->rescaleAxes();
                ui->qcustomplot->replot();
            });
    menu.exec(QCursor::pos());
}

void Widget::contextMenuEvent(QContextMenuEvent *event)  //重写菜单右键函数
{
    Q_UNUSED(event);  //避免编译器警告
    QMenu menu;
    //添加右键菜单的选项
    QAction *p1 = menu.addAction("方法1：选项1");
    connect(p1,&QAction::triggered,
            [=]()
            {
                qDebug() << "选项1 被点击";
            }
            );
    menu.addAction("方法1：选项2");
    menu.addAction("方法1：选项3");
    //显示menu菜单并设置其显示位置为鼠标位置
    menu.exec(QCursor::pos());
}

void Widget::on_tableViewCustomContextMenuRequested(const QPoint &pos)  // 右键菜单槽函数
{
    //拓展：此处的pos函数可以使用QTableView的indexAt
    //函数获取当前鼠标下控件的QModelIndex对象（可用于判断操作）
    Q_UNUSED(pos);
    QMenu menu;
    //添加右键菜单的选项
    QAction *p1 = menu.addAction("恢复原状");
    connect(p1,&QAction::triggered,
            [=]()
            {
                ui->widget->graph(0)->rescaleAxes();
                ui->widget->replot();
            }
    );
    menu.addAction("方法2：选项2");
    //显示menu菜单并设置其显示位置为鼠标位置
    menu.exec(QCursor::pos());
}

void Widget::drawImage()
{
//        QVector<double> x(101), y(101); //初始化向量x和y
//        for (int i=0; i<101; ++i)
//        {
//          x[i] = i/50.0 - 1; // x范围[-1,1]
//          y[i] = x[i]*x[i]; // y=x*x
//        }
//        ui->widget->addGraph();//添加数据曲线（一个图像可以有多个数据曲线）
//        // graph(0);可以获取某个数据曲线（按添加先后排序）
//        // setData();为数据曲线关联数据
//        ui->widget->graph(0)->setData(x, y);
        ui->widget->setBackground(QColor(37, 37, 38));
        // 设置x/y轴文本色、轴线色、字体等
        ui->widget->xAxis->setTickLabelColor(Qt::white);
        ui->widget->xAxis->setLabelColor(QColor(0, 160, 230));
        ui->widget->xAxis->setBasePen(QPen(Qt::white, 2));
        ui->widget->xAxis->setTickPen(QPen(QColor(128, 0, 255)));
        ui->widget->xAxis->setSubTickPen(QColor(255, 165, 0));

        ui->widget->yAxis->setTickLabelColor(Qt::white);
        ui->widget->yAxis->setLabelColor(QColor(0, 160, 230));
        ui->widget->yAxis->setBasePen(QPen(QColor(32, 178, 170)));
        ui->widget->yAxis->setTickPen(QPen(QColor(128, 0, 255)));
        ui->widget->yAxis->setSubTickPen(QColor(255, 165, 0));
        ui->widget->addGraph();
        QVector<double> xx;
        QVector<double> data;
        ui->widget->xAxis->setRange(0,2000);
        ui->widget->yAxis->setRange(0, 100);
        for(int i=0;i<2000;i++)
        {
            xx <<i;
            data << (qrand() % 100);
        };
        ui->widget->graph(0)->setData(xx,data);
        ui->widget->graph(0)->setName("示例1");// 设置图例名称
        ui->widget->legend->setVisible(true); // 显示图例  legend 图例
        // 支持鼠标拖拽轴的范围、滚动缩放轴的范围，左键点选图层（每条曲线独占一个图层）
        //ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        //设置框选放大
        ui->widget->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
        //设置框选边框为虚线
        ui->widget->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));
        //设置框选范围半透明浅蓝
        ui->widget->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));
        //自动调整XY轴的范围，以便显示出graph(0)中所有的点
        ui->widget->graph(0)->rescaleAxes();

}

Widget::~Widget()
{
    delete ui;
}

