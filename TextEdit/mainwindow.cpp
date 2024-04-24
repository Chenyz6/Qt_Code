#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("信道编码处理软件");
    resize(1250, 832);          // 设置主窗口大小
    InitTimer();                // 初始化定时器
    CreateChildWidget();        // 创建子窗口并显示
    SetTabbedView();            // MDI模式显示
    InitProgress();             // 初始化状态栏进度条
    InitPara();                 // 初始化参数
    connect(textEdit, &TextEdit::SendThreadFinish, this, &MainWindow::ReceiveThreadFinish);
    connect(textEdit, &TextEdit::filePath, this,
            [=](){
                QString str = "信道编码处理软件--";
                setWindowTitle(str += openFilePara.fileAbsolutePath);
                ui->lineEdit_bitfilename_coding->setText(openFilePara.fileAbsolutePath);
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 创建子窗口并显示
void MainWindow::CreateChildWidget()
{
    // 创建文件显示子窗口
    textEdit = new TextEdit(ui->mdiArea);	// 新建文件显示子窗口
    ui->mdiArea->addSubWindow(textEdit);	// 将该界面加入多文档子窗口
    textEdit->setWindowTitle("名称1");       // 设置窗口名称
    textEdit->show();
}

// MDI模式显示
void MainWindow::SetTabbedView()
{
    ui->mdiArea->setViewMode(QMdiArea::TabbedView);     // Tab方式
//    ui->mdiArea->setTabsClosable(false);                // 设置页面不可关闭
//    ui->mdiArea->setTabPosition(QTabWidget::South);     // 设置标题栏在下方显示
}

// 初始化定时器
void MainWindow::InitTimer()
{
    // 初始化状态栏时间定时器
    timer = new QTimer;
    connect(timer, &QTimer::timeout, this, &MainWindow::UpdateTime); // 设置状态栏显示时间
    timer->start(1000);
}

// 初始化参数
void MainWindow::InitPara()
{
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // 工具栏文字显示在图标下面
    ui->statusbar->addWidget(LTime, 0);     // 状态栏显示时间标签
    ui->statusbar->addPermanentWidget(progressBar,0);   // 状态栏末尾添加进度条
    ui->stackedWidget->setCurrentIndex(0);  // 默认显示第一页
}

// 初始化状态栏进度条
void MainWindow::InitProgress()
{
    progressBar->setRange(0,100);
    progressBar->setValue(0);
    progressBar->setFixedWidth(240);
    connect(progressBarTimer, &QTimer::timeout, this,
            [=](){
//                QString str = "Progress:";
//                str += QString::number(ProgressPercent,'f', 4);
//                str += "%";
//                ProgressPercentLable->setText(str);
                progressBar->setValue(ProgressPercent);
            });
    timer->start(500);
}

// 状态栏刷新显示时间
void MainWindow::UpdateTime()
{
    ui->statusbar->setSizeGripEnabled(false);
    //ui->statusbar->setStyleSheet("color:rgb(255, 255, 255)");
    ui->statusbar->setStyleSheet("color:rgb(0,0,0)");
    QString Stime;
    QDateTime Timedata = QDateTime::currentDateTime();
    Stime = Timedata.toString("yyyy-MM-dd hh:mm:ss");
    LTime->setText(Stime);
}

// 启动帧结构检测
void MainWindow::on_pushButton_framestructuredetection_clicked()
{
    if(ui->lineEdit_bitfilename_coding->text() == ""){
        QMessageBox::warning(0, "Warning", "请输入01码文件路径（.dat）!");
        return;
    }
    ui->pushButton_framestructuredetection->setEnabled(false);
    ui->pushButton_framestructuredetection->setText("正在进行帧结构检测...");
    ChannelCodingPara *channelCodingPara = new ChannelCodingPara;
    channelCodingPara->range = ui->lineEdit_range->text(); // 帧结构范围
    bitFileNameCoding = ui->lineEdit_bitfilename_coding->text();
    textEdit->OpenFrameStructureDetection(channelCodingPara);
}

// 启动DESSR编码
void MainWindow::on_pushButton_DESSR_clicked()
{
    if(ResultDataLen == 0){
        QMessageBox::warning(0, "Warning", "请先进行帧结构检测!");
        return;
    }
    ui->pushButton_DESSR->setEnabled(false);
    ui->pushButton_DESSR->setText("正在进行DESSR解码...");
    ChannelCodingPara* channelCodingPara = new ChannelCodingPara;
    channelCodingPara->dessr_g = ui->lineEdit_dessr_g->text(); // 生成多项式
    bitFileNameCoding = ui->lineEdit_bitfilename_coding->text();
    textEdit->OpenDESSR(channelCodingPara);
}

// 启动RS编码
void MainWindow::on_pushButton_RS_clicked()
{
    if(ResultDataLen == 0){
        QMessageBox::warning(0, "Warning", "请先进行帧结构检测!");
        return;
    }
    ui->pushButton_RS->setEnabled(false);
    ui->pushButton_RS->setText("正在进行RS解码...");
    ChannelCodingPara* channelCodingPara = new ChannelCodingPara;
    channelCodingPara->rs_m = ui->lineEdit_rs_m->text().toInt(); // 2的幂次
    channelCodingPara->rs_n = ui->lineEdit_rs_n->text().toInt(); // 码长
    channelCodingPara->rs_k = ui->lineEdit_rs_k->text().toInt(); // 信息码长
    channelCodingPara->rs_g = ui->lineEdit_rs_g->text(); // 生成多项式
    bitFileNameCoding = ui->lineEdit_bitfilename_coding->text();
    textEdit->OpenRS(channelCodingPara);
}

// 启动CONV编码
void MainWindow::on_pushButton_CONV_clicked()
{
    if(ResultDataLen == 0){
        QMessageBox::warning(0, "Warning", "请先进行帧结构检测!");
        return;
    }
    ui->pushButton_CONV->setEnabled(false);
    ui->pushButton_CONV->setText("正在进行CONV解码...");
    ChannelCodingPara* channelCodingPara = new ChannelCodingPara;
    channelCodingPara->conv_m = ui->lineEdit_conv_m->text().toInt(); // 约束长度
    channelCodingPara->conv_n = ui->lineEdit_conv_n->text().toInt(); // 码长
    channelCodingPara->conv_k = ui->lineEdit_conv_k->text().toInt(); // 信息码长
    channelCodingPara->conv_pins = ui->lineEdit_conv_pins->text(); // 生成多项式
    bitFileNameCoding = ui->lineEdit_bitfilename_coding->text();
    textEdit->OpenCONV(channelCodingPara);
}

// 启动BCH编码
void MainWindow::on_pushButton_BCH_clicked()
{
    if(ResultDataLen == 0){
        QMessageBox::warning(0, "Warning", "请先进行帧结构检测!");
        return;
    }
    ui->pushButton_BCH->setEnabled(false);
    ui->pushButton_BCH->setText("正在进行BCH解决码...");
    ChannelCodingPara* channelCodingPara = new ChannelCodingPara;
    channelCodingPara->bch_n = ui->lineEdit_bch_n->text().toInt(); // 码长
    channelCodingPara->bch_k = ui->lineEdit_bch_k->text().toInt(); // 信息码长
    channelCodingPara->bch_pins = ui->lineEdit_bch_pins->text(); // 生成多项式
    bitFileNameCoding = ui->lineEdit_bitfilename_coding->text();
    textEdit->OpenBCH(channelCodingPara);
}

// TPC
void MainWindow::on_pushButton_TPC_clicked()
{
    if(ResultDataLen == 0){
        QMessageBox::warning(0, "Warning", "请先进行帧结构检测!");
        return;
    }
    ui->pushButton_TPC->setEnabled(false);
    ui->pushButton_TPC->setText("正在进行TPC...");
    ChannelCodingPara* channelCodingPara = new ChannelCodingPara;
    channelCodingPara->tpc_n = ui->lineEdit_tpc_n->text().toInt(); // 码长
    channelCodingPara->tpc_k = ui->lineEdit_tpc_k->text().toInt(); // 信息码长
    channelCodingPara->tpc_pins = ui->lineEdit_tpc_pins->text(); // 生成多项式
    bitFileNameCoding = ui->lineEdit_bitfilename_coding->text();
    textEdit->OpenTPC(channelCodingPara);
}

// LDPC
void MainWindow::on_pushButton_ldpc_clicked()
{
    if(ResultDataLen == 0){
        QMessageBox::warning(0, "Warning", "请先进行帧结构检测!");
        return;
    }
    ui->pushButton_ldpc->setEnabled(false);
    ui->pushButton_ldpc->setText("正在进行LDPC...");
    ChannelCodingPara* channelCodingPara = new ChannelCodingPara;
    channelCodingPara->ldpc_n = ui->lineEdit_ldpc_n->text().toInt(); // 码长
    channelCodingPara->ldpc_k = ui->lineEdit_ldpc_k->text().toInt(); // 信息码长
    bitFileNameCoding = ui->lineEdit_bitfilename_coding->text();
    int ldpc_n = ui->lineEdit_ldpc_n->text().toInt();
    int ldpc_k = ui->lineEdit_ldpc_k->text().toInt();
    textEdit->OpenLdpc(channelCodingPara, ldpc_n, ldpc_k);
}

// 编码识别
void MainWindow::on_pushButton_opencodingidentify_clicked()
{
    if(ResultDataLen == 0){
        QMessageBox::warning(0, "Warning", "请先进行帧结构检测!");
        return;
    }
    ui->pushButton_opencodingidentify->setEnabled(false);
    ui->pushButton_opencodingidentify->setText("正在进行编码识别...");
    textEdit->OpenCodingIdentify();
}

// 上一页
void MainWindow::on_pushButton_codinglastpage_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

// 下一页
void MainWindow::on_pushButton_codingnextpage_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// 工具栏点击打开文件
void MainWindow::on_action_Open_triggered()
{
    textEdit->OpenFile();
}

// 接收到线程处理完毕
void MainWindow::ReceiveThreadFinish(short index)
{
    switch(index){
    case 0: // 帧检测结构
    {
        ui->pushButton_framestructuredetection->setEnabled(true);
        ui->pushButton_framestructuredetection->setText("启动帧结构检测");
    }break;
    case 1: // DESSR编码
    {
        ui->pushButton_DESSR->setEnabled(true);
        ui->pushButton_DESSR->setText("启动DESSR解码");
    }break;
    case 2: // RS编码
    {
        ui->pushButton_RS->setEnabled(true);
        ui->pushButton_RS->setText("启动RS解码");
    }break;
    case 3: // CONV编码 卷积码
    {
        ui->pushButton_CONV->setEnabled(true);
        ui->pushButton_CONV->setText("启动CONV解码");
    }break;
    case 4: // BCH编码
    {
        ui->pushButton_BCH->setEnabled(true);
        ui->pushButton_BCH->setText("启动BCH解码");
    }break;
    case 5: // TPC
    {
        ui->pushButton_TPC->setEnabled(true);
        ui->pushButton_TPC->setText("启动TPC");
    }break;
    case 6: // LDPC
    {
        ui->pushButton_ldpc->setEnabled(true);
        ui->pushButton_ldpc->setText("启动LDPC");
    }break;
    case 7: // 编码识别
    {
        ui->pushButton_opencodingidentify->setEnabled(true);
        ui->pushButton_opencodingidentify->setText("启动编码识别");
        //return:识别类型
        //1:BCH
        //2:RS
        //3:系统卷积码
        //4:非系统卷积码
        //5:LDPC
        //6:扰码
        ui->comboBox_identifyresult->setCurrentIndex(IdentifyResult);
        QMessageBox::warning(0, "Warning", "识别成功，请看识别结果!");
    }break;
    default:{

    }
    }
}



