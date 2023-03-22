#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("绘图");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *)
{
    //QPainter p(this);

    QPainter p;
    p.begin(this);

    QPen pen; //定义画笔
    pen.setWidth(5);  //设置线宽
    pen.setStyle(Qt::DashLine); //设置风格
    //pen.setColor(Qt::red);
    pen.setColor(QColor(0,0,255));
    //p.drawPixmap(0,0,width(),height(),QPixmap("../image/2.png"));
    //画背景图
    //p.drawPixmap(rect(),QPixmap("../image/2.png"));
    p.setPen(pen);  //把画笔交给画家
    //画直线
    p.drawLine(50,50,100,100);
    p.drawLine(50,50,50,150);

    //创建画刷对象
    QBrush brush;
    brush.setColor(Qt::red);
    brush.setStyle(Qt::Dense1Pattern);
    p.setBrush(brush);
    //画矩形
    p.drawRect(400,400,100,50);
    //画圆形
    p.drawEllipse(QPoint(200,200),50,25);    //Ellipse-椭圆
    //50-水平半径   25-垂直半径
    p.drawPixmap(x,200,200,200,QPixmap("../image/2.png"));

    p.end();
}




void Widget::on_pushButton_clicked()
{
    x += 30;
    if(x > width())
    {
        x = 0;
    }

    update();  //  用于刷新  重绘整个窗口
               //  但是也可以进行指定大小进行刷新
               //  间接调用paintEvent
}
