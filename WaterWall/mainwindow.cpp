#include "mainwindow.h"
#include "ui_mainwindow.h"
QVector<double> Waterfall_XData;				//瀑布图x轴数据点数
QVector<double> Waterfall_YData;				//瀑布图y轴数据点数
QList<QVector<double> > value_lofar;			//瀑布图数据
int nWFWndWidth = 0;							//瀑布图宽度

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1300,800);
    init_Waterfall();       // 初始化瀑布图
    init_Constellation();   // 初始化星座图
    ui->DirFind->installEventFilter(this);		//注册事件过滤器
    connect(time2,&QTimer::timeout,this,
            [=]()
            {
//                nAzimuth = qrand() % 360;
                ui->DirFind->update();
            });
    time2->start(500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_Constellation()
{
    //设置范围
    ui->Constellation->xAxis->setRange(-50, 50);
    ui->Constellation->yAxis->setRange(-50, 50);

    //设置网格线和零刻度线颜色
    QPen ZeroLine;
    ZeroLine.setColor(QColor(255, 255, 255));
    ZeroLine.setWidth(1);
    ui->Constellation->xAxis->grid()->setZeroLinePen(ZeroLine);
    ui->Constellation->yAxis->grid()->setZeroLinePen(ZeroLine);
    ui->Constellation->xAxis->setBasePen(QPen(QColor(37, 37, 38)));
    ui->Constellation->xAxis->setTickPen(QPen(QColor(37, 37, 38)));
    ui->Constellation->xAxis->setSubTickPen(QPen(QColor(37, 37, 38)));
    ui->Constellation->xAxis->setVisible(true);
    ui->Constellation->xAxis->setTickLabels(false);

    //设置背景色
    ui->Constellation->setBackground(QColor(37, 37, 38));

    //添加曲线
    ui->Constellation->addGraph();

    //画笔风格
    QPen drawPen;
    drawPen.setColor(QColor(0, 255, 255));

    //绘图方式
    ui->Constellation->graph(0)->setLineStyle(QCPGraph::LineStyle::lsNone);	//图形连接方式：无连线（散点）
    ui->Constellation->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 0.8));//圆形实心点，大小为0.8
    ui->Constellation->graph(0)->setPen(drawPen);
    connect(time1,&QTimer::timeout,this,
            [=]()
            {
                QVector<double> ydata,xdata;
                for(int i  = -50;i<50;i++)
                {
                    ydata << qrand() % 50;
                    xdata << i;
                }
                for(int i  = -50;i<50;i++)
                {
                    ydata << -(qrand() % 50);
                    xdata << i;
                }
                for(int i  = -50;i<50;i++)
                {
                    ydata << qrand() % 50;
                    xdata << i;
                }
                for(int i  = -50;i<50;i++)
                {
                    ydata << -(qrand() % 50);
                    xdata << i;
                }
                for(int i  = -50;i<50;i++)
                {
                    ydata << qrand() % 50;
                    xdata << i;
                }
                for(int i  = -50;i<50;i++)
                {
                    ydata << -(qrand() % 50);
                    xdata << i;
                }
                ui->Constellation->graph(0)->setData(xdata, ydata);
                ui->Constellation->replot(QCustomPlot::rpQueuedReplot);
            });
    time1->start(100);
}

void MainWindow::init_Waterfall()
{
    ui->Waterfall->setBackground(QColor(37, 37, 38));	    //设置背景颜色(黑)
    ui->Waterfall->xAxis->setVisible(false);				//使x轴网格线不可见
    ui->Waterfall->yAxis->setVisible(false);				//使y轴网格线不可见


    m_pColorMap = new QCPColorMap(ui->Waterfall->xAxis, ui->Waterfall->yAxis);
//    m_pColorMap->data()->setSize(1001, 70 + 1);									//设置整个图的x,y点数
//    m_pColorMap->data()->setRange(QCPRange(0, 1001), QCPRange(0, 70 + 1));		/的/设置x轴和y轴范围


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
//    ui->Waterfall->rescaleAxes();									//自适应大小



    // 设置颜色图的数据范围
    int nx = 101; // x 轴方向数据点个数
    int ny = 51; // y 轴方向数据点个数
    m_pColorMap->data()->setSize(nx, ny);
    m_pColorMap->data()->setRange(QCPRange(0, nx), QCPRange(0, 51));
    m_pColorMap->setDataRange(QCPRange(0, 100));

    connect(time,&QTimer::timeout,this,
            [=]()
            {
                if(value_lofar.size()>49)
                {
                    value_lofar.removeLast();  //当lofar累积到了50个，删除最后面的数据，防止绘图溢出绘图区域
                }
                QVector<double> data;
                for(int i  = 0;i<100;i++)
                {
                    data << qrand() % 100;
                }
                value_lofar.prepend(data);//新来的数据一直往前面累加

                for (int i=0;i<value_lofar.size();i++)
                {
                    for(int j=0;j<100;j++)
                    {
                        m_pColorMap->data()->setCell(j,i,value_lofar[i][j]);
                    }
                }
                ui->Waterfall->replot();
            });

    time->start(100);
}

//事件过滤器：用来过滤掉父窗口的绘图事件，使绘图事件在选定子窗口实现
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->DirFind && event->type() == QEvent::Paint)
    {
        init_DirectionFind();	//初始化测向图
        return true;	//手动处理
    }
    else
        return false;	//继续执行
}

void MainWindow::init_DirectionFind()
{
    //设置背景
    QPalette Bgpal(ui->DirFind->palette());
    Bgpal.setColor(QPalette::Background, Qt::black);
    ui->DirFind->setAutoFillBackground(true);
    ui->DirFind->setPalette(Bgpal);

    //Widget中心点
    int nWidth = ui->DirFind->width();
    int nHeight = ui->DirFind->height();
    int radius = ((nWidth > nHeight) ? nHeight : nWidth) / 2;	//按矩形最小宽度取半径

    QPainter Painter(ui->DirFind);
    //将画笔移动到中下方
    Painter.translate(nWidth>>1, nHeight*0.5);	//图形正中央
    //启用反锯齿
    Painter.setRenderHint(QPainter::Antialiasing, true);
    //取消画笔
    Painter.setPen(Qt::NoPen);
    //设置画笔颜色
    Painter.setBrush(QColor(98, 246, 255));

    /*绘制子弹头*/
    //组装点的路径
    QPainterPath PointPath;
    PointPath.moveTo(-2, -2);
    PointPath.lineTo(2, -2);
    PointPath.lineTo(2, 2);
    PointPath.lineTo(0, 4);
    PointPath.lineTo(-2, 2);
    //绘制18个小点
    for (int i = 0; i < 18; i++)
    {
        QPointF Point(0, 0);
        Painter.save();			//保存当前坐标系
        //计算并移动绘图对象中心点
        Point.setX(radius*qCos(((-90 - i * 20)*M_PI) / 180));
        Point.setY(radius*qSin(((-90 - i * 20)*M_PI) / 180));
        Painter.translate(Point.x(), -Point.y());	//转移坐标系原点
        //计算并选择绘图对象坐标
        Painter.rotate(-180 + i * 20);	//顺时针旋转坐标系
        //绘制路径
        Painter.drawPath(PointPath);
        Painter.restore();	//复原坐标系
    }

    /*绘制内测数字*/
    //半径缩小10
    radius -= 10;
    Painter.setPen(QColor(98, 246, 255));
    //设置字体
    QFont Font;
    Font.setFamily("SimHei");
    Font.setPointSize(16);
    Painter.setFont(Font);
    //绘制18个数字
    for (int i = 0; i < 18; i++)
    {
        QPointF Point(0, 0);
        Painter.save();			//保存当前坐标系
        //计算并移动绘图对象中心点
        Point.setX(radius*qCos(((90 - i * 20)*M_PI) / 180));
        Point.setY(radius*qSin(((90 - i * 20)*M_PI) / 180));
        Painter.translate(Point.x(), -Point.y());	//转移坐标系原点
        //计算并选择绘图对象坐标
        Painter.rotate(i * 20);	//顺时针旋转坐标系
        //绘制路径
        QString str = QString::number(i * 20) + "°";
        Painter.drawText(-15, 0, 50, 20, Qt::AlignCenter, str);
        Painter.restore();	//复原坐标系
    }
    //去掉画笔
    Painter.setPen(Qt::NoPen);

    /*绘制渐变圆环*/
    //半径缩小25
    radius -= 25;
    Painter.save();
    //计算大小圆路径
    QPainterPath outRing;
    QPainterPath inRing;
    outRing.moveTo(0, 0);
    inRing.moveTo(0, 0);
    outRing.arcTo(-radius, -radius, 2 * radius, 2 * radius, 0, 360);
    inRing.addEllipse(-radius + 20, -radius + 20, 2 * (radius - 20), 2 * (radius - 20));
    outRing.closeSubpath();
    //设置渐变色
    QRadialGradient radialGradient(0, 0, radius, 0, 0);
    radialGradient.setColorAt(0.95, QColor(98, 246, 255));
    radialGradient.setColorAt(1, QColor(0, 0, 0));
    //设置渐变画刷
    Painter.setBrush(radialGradient);
    //大圆减去小圆得到圆环
    Painter.drawPath(outRing.subtracted(inRing));
    //恢复绘图对象
    Painter.restore();

    /*绘制小刻度*/
    //半径缩小25
    radius -= 25;
    //组装点的路径图
    QPainterPath PointPath2;
    PointPath2.moveTo(-2, -2);
    PointPath2.lineTo(-1, -4);
    PointPath2.lineTo(1, -4);
    PointPath2.lineTo(2, -2);
    PointPath2.lineTo(1, 8);
    PointPath2.lineTo(-1, 8);
    //绘制小点
    for (int i = 0; i < 180; i++)
    {
        QPointF Point(0, 0);
        Painter.save();
        //计算并移动绘图对象中心点
        Point.setX(radius*qCos(((-90 - i * 2)*M_PI) / 180));
        Point.setY(radius*qSin(((-90 - i * 2)*M_PI) / 180));
        Painter.translate(Point.x(), -Point.y());
        //计算并选择绘图对象坐标
        Painter.rotate(-180 + i * 2);
        //绘制路径
        Painter.drawPath(PointPath2);
        Painter.restore();
    }

    /*绘制大刻度*/
    //半径缩小15
    radius -= 15;
    //组装散点路径图
    //组装点的路径图
    QPainterPath pointPath3;
    pointPath3.moveTo(-2, -2);
    pointPath3.lineTo(-1, -4);
    pointPath3.lineTo(1, -4);
    pointPath3.lineTo(2, -2);
    pointPath3.lineTo(1, 8);
    pointPath3.lineTo(-1, 8);
    QPainterPath pointPath4;
    pointPath4.moveTo(-2, -2);
    pointPath4.lineTo(-1, -4);
    pointPath4.lineTo(1, -4);
    pointPath4.lineTo(2, -2);
    pointPath4.lineTo(1, 15);
    pointPath4.lineTo(-1, 15);
    //绘制36个刻度
    for (int i = 0; i < 36; i++)
    {
        QPointF point(0, 0);
        Painter.save();
        //计算并移动绘图对象中心点
        point.setX(radius*qCos(((-90 - i * 10)*M_PI) / 180));
        point.setY(radius*qSin(((-90 - i * 10)*M_PI) / 180));
        Painter.translate(point.x(), -point.y());
        //选择并计算绘图对象坐标
        Painter.rotate(-180 + i * 10);
        //绘制路径
        if (i % 2)
            Painter.drawPath(pointPath3);
        else
            Painter.drawPath(pointPath4);
        Painter.restore();
    }

    /*绘制当前方位角*/
    //半径还原为原来的一半
    radius += 75;
    radius /= 2;
    Painter.save();
    //设置画笔
    Painter.setPen(QColor(98, 246, 255));
    //设置字体
    QFont Font2;
    Font2.setFamily("Microsoft YaHei");
    Font2.setPixelSize(25);
    Painter.setFont(Font2);
    QString AzimuthValue = QString::number(nAzimuth) + "°";
    Painter.drawText(-50, -radius, 100, 20, Qt::AlignCenter, AzimuthValue);
    Painter.restore();

    /*绘制指针*/
    radius *= 2;
    radius -= 100;
    //组装点的路径图
    QPainterPath pointPath5;
    pointPath5.moveTo(10, 0);
    pointPath5.lineTo(1, -radius);
    pointPath5.lineTo(-1, -radius);
    pointPath5.lineTo(-10, 0);
    pointPath5.arcTo(-10, 0, 20, 20, 180, 180);
    QPainterPath inRing2;
    inRing2.addEllipse(-5, -5, 10, 10);
    Painter.save();
    //计算并选择绘图对象坐标
    Painter.rotate(nAzimuth);
    //设置画刷
    Painter.setBrush(QColor(255, 0, 0, 150));
    //绘制路径
    Painter.drawPath(pointPath5.subtracted(inRing2));
    Painter.restore();
}

