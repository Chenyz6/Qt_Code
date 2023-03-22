#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QMenuBar>  //菜单栏
#include<QMenu>  //菜单
#include<QAction>
#include<QDebug>
#include<QToolBar>
#include<QPushButton>
#include<QStatusBar>  //状态栏
#include<QLabel>
#include<QTextEdit>  //文本编辑
#include<QDockWidget>  //浮动窗口


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    resize(500,500);
    setWindowTitle("菜单的显示");
    //菜单栏
    QMenuBar *mBar = menuBar(); //内部会给分配 所以不需要new
    //添加菜单
    QMenu *pFile = mBar->addMenu("文件");
    //添加菜单项 添加动作
    QAction *pNew = pFile->addAction("新建");
    connect(pNew, &QAction::triggered,
            [=]()
            {
                qDebug() << "按键被按下";
            }
            );
    pFile->addSeparator();  //添加分割线
    QAction *pOpen = pFile->addAction("打开");
    //工具栏  菜单项的快捷方式
    QToolBar *toolbar = addToolBar("toolBar");
    //工具栏添加快捷键
    toolbar->addAction(pNew);
    QPushButton *b = new QPushButton(this);
    b->setText("^_^");
    //添加小控件
    toolbar->addWidget(b);
    connect(b, &QPushButton::clicked,
            [=]()
            {
                b->setText("123");
            });
    //状态栏
    QStatusBar *statusbar = statusBar();
    QLabel *label = new QLabel(this);
    label->setText("Normal text file");
    statusbar->addWidget(label);
    //addWidget  从左往右添加
    statusbar->addWidget(new QLabel("2",this));
    statusbar->addPermanentWidget(new QLabel("3",this));
    //addPermanentWidget  从右往左添加
    //核心控件
    QTextEdit *textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);
    //浮动窗口
    QDockWidget *dock = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea,dock);
    //浮动窗口添加
    QTextEdit *textEdit1 = new QTextEdit(this);
    dock->setWidget(textEdit1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

