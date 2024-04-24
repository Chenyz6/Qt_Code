#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("YoloV5目标检测软件"));

    timer = new QTimer(this);
    timer->setInterval(33);
    connect(timer,SIGNAL(timeout()),this,SLOT(readFrame()));
    ui->startdetect->setEnabled(false);
    ui->stopdetect->setEnabled(false);
    Init();
}

MainWindow::~MainWindow()
{

    capture->release();
    delete capture;
    delete [] yolo_nets;
    delete yolov5;
    delete ui;
}

void MainWindow::Init()
{
    capture = new cv::VideoCapture();
    yolo_nets = new NetConfig[4]{
                                {0.5, 0.5, 0.5, "yolov5s"},
                                {0.6, 0.6, 0.6, "yolov5m"},
                                {0.65, 0.65, 0.65, "yolov5l"},
                                {0.75, 0.75, 0.75, "yolov5x"}
                            };
    conf = yolo_nets[0];
    yolov5 = new YOLOV5();
    yolov5->Initialization(conf);
    ui->textEditlog->append(QStringLiteral("默认模型类别：yolov5s args: %1 %2 %3")
                            .arg(conf.nmsThreshold)
                            .arg(conf.objThreshold)
                            .arg(conf.confThreshold));
}

void MainWindow::readFrame()
{
    cv::Mat frame;
    capture->read(frame);
    if (frame.empty()) return;

    auto start = std::chrono::steady_clock::now();
    yolov5->detect(frame);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    ui->textEditlog->append(QString("cost_time: %1 ms").arg(elapsed.count()));
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage rawImage = QImage((uchar*)(frame.data),frame.cols,frame.rows,frame.step,QImage::Format_RGB888);
    // 自适应大小
    QPixmap pixmap = QPixmap::fromImage(rawImage);
    pixmap = pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label->setPixmap(pixmap);
}

void MainWindow::on_openfile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,QStringLiteral("打开文件"),".","*.mp4 *.avi;;*.png *.jpg *.jpeg *.bmp");
    if(!QFile::exists(filename)){
        return;
    }
    ui->statusbar->showMessage(filename);

    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filename);
    if (mime.name().startsWith("image/")) {
        cv::Mat src = cv::imread(filename.toLatin1().data());
        if(src.empty()){
            ui->statusbar->showMessage("图像不存在！");
            return;
        }
        cv::Mat temp;
        if(src.channels()==4)
            cv::cvtColor(src,temp,cv::COLOR_BGRA2RGB);
        else if (src.channels()==3)
            cv::cvtColor(src,temp,cv::COLOR_BGR2RGB);
        else
            cv::cvtColor(src,temp,cv::COLOR_GRAY2RGB);

        auto start = std::chrono::steady_clock::now();
        yolov5->detect(temp);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        ui->textEditlog->append(QString("cost_time: %1 ms").arg(elapsed.count()));
        QImage img = QImage((uchar*)(temp.data),temp.cols,temp.rows,temp.step,QImage::Format_RGB888);
        ui->label->setPixmap(QPixmap::fromImage(img));
        ui->label->resize(ui->label->pixmap()->size());
        filename.clear();
    }else if (mime.name().startsWith("video/")) {
        capture->open(filename.toLatin1().data());
        if (!capture->isOpened()){
            ui->textEditlog->append("fail to open MP4!");
            return;
        }
        IsDetect_ok +=1;
        if (IsDetect_ok ==2)
            ui->startdetect->setEnabled(true);
        ui->textEditlog->append(QString::fromUtf8("Open video: %1 succesfully!").arg(filename));

        //获取整个帧数QStringLiteral
        long totalFrame = capture->get(cv::CAP_PROP_FRAME_COUNT);
        int width = capture->get(cv::CAP_PROP_FRAME_WIDTH);
        int height = capture->get(cv::CAP_PROP_FRAME_HEIGHT);
        ui->textEditlog->append(QStringLiteral("整个视频共 %1 帧, 宽=%2 高=%3 ").arg(totalFrame).arg(width).arg(height));
        ui->label->resize(QSize(width, height));

        //设置开始帧()
        long frameToStart = 0;
        capture->set(cv::CAP_PROP_POS_FRAMES, frameToStart);
        ui->textEditlog->append(QStringLiteral("从第 %1 帧开始读").arg(frameToStart));

        //获取帧率
        double rate = capture->get(cv::CAP_PROP_FPS);
        ui->textEditlog->append(QStringLiteral("帧率为: %1 ").arg(rate));
    }
}

void MainWindow::on_loadfile_clicked()
{
    QString onnxFile = QFileDialog::getOpenFileName(this,QStringLiteral("选择模型"),".","*.onnx");
    if(!QFile::exists(onnxFile)){
        return;
    }
    ui->statusbar->showMessage(onnxFile);
    if (!yolov5->loadModel(onnxFile.toLatin1().data())){
        ui->textEditlog->append(QStringLiteral("加载模型失败！"));
        return;
    }
    IsDetect_ok +=1;
    ui->textEditlog->append(QString::fromUtf8("Open onnxFile: %1 succesfully!").arg(onnxFile));
    if (IsDetect_ok ==2)
        ui->startdetect->setEnabled(true);
}

void MainWindow::on_startdetect_clicked()
{
    timer->start();
    ui->startdetect->setEnabled(false);
    ui->stopdetect->setEnabled(true);
    ui->openfile->setEnabled(false);
    ui->loadfile->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->textEditlog->append(QStringLiteral("=======================\n"
                                           "        开始检测\n"
                                           "=======================\n"));
}

void MainWindow::on_stopdetect_clicked()
{
    ui->startdetect->setEnabled(true);
    ui->stopdetect->setEnabled(false);
    ui->openfile->setEnabled(true);
    ui->loadfile->setEnabled(true);
    ui->comboBox->setEnabled(true);
    timer->stop();
    ui->textEditlog->append(QStringLiteral("======================\n"
                                           "        停止检测\n"
                                           "======================\n"));
}

void MainWindow::on_comboBox_activated(const QString &arg1)
{
    if (arg1.contains("s")){
        conf = yolo_nets[0];
    }else if (arg1.contains("m")) {
        conf = yolo_nets[1];
    }else if (arg1.contains("l")) {
        conf = yolo_nets[2];
    }else if (arg1.contains("x")) {
        conf = yolo_nets[3];}
    yolov5->Initialization(conf);
    ui->textEditlog->append(QStringLiteral("使用模型类别：%1 args: %2 %3 %4")
                            .arg(arg1)
                            .arg(conf.nmsThreshold)
                            .arg(conf.objThreshold)
                            .arg(conf.confThreshold));
}
