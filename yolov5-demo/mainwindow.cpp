#include "mainwindow.h"
#include "ui_mainwindow.h"

// 构造函数：MainWindow类的构造函数
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this); // 设置UI界面
    setWindowTitle(QStringLiteral("YoloV5目标检测软件")); // 设置窗口标题

    timer = new QTimer(this); // 创建定时器对象
    timer->setInterval(33); // 设置定时器间隔为33毫秒，约30帧/秒
    connect(timer,SIGNAL(timeout()),this,SLOT(readFrame())); // 将定时器的timeout信号连接到readFrame槽函数
    ui->startdetect->setEnabled(false); // 设置开始检测按钮不可用
    ui->stopdetect->setEnabled(false); // 设置停止检测按钮不可用
    Init(); // 调用初始化函数
}

// 析构函数：MainWindow类的析构函数
MainWindow::~MainWindow()
{
    capture->release(); // 释放视频捕获对象
    delete capture; // 删除视频捕获对象
    delete [] yolo_nets; // 删除YOLO网络配置数组
    delete yolov5; // 删除YOLOv5对象
    delete ui; // 删除UI界面
}

// 初始化函数：MainWindow类的初始化函数
void MainWindow::Init()
{
    capture = new cv::VideoCapture(); // 创建视频捕获对象
    yolo_nets = new NetConfig[4]{ // 创建YOLO网络配置数组
                                {0.5, 0.5, 0.5, "yolov5s"},
                                {0.6, 0.6, 0.6, "yolov5m"},
                                {0.65, 0.65, 0.65, "yolov5l"},
                                {0.75, 0.75, 0.75, "yolov5x"}
                            };
    conf = yolo_nets[0]; // 默认使用yolov5s模型配置
    yolov5 = new YOLOV5(); // 创建YOLOv5对象
    yolov5->Initialization(conf); // 初始化YOLOv5对象
    // 在UI界面的文本框中显示默认模型类别及参数
    ui->textEditlog->append(QStringLiteral("默认模型类别：yolov5s args: %1 %2 %3")
                            .arg(conf.nmsThreshold)
                            .arg(conf.objThreshold)
                            .arg(conf.confThreshold));
}

// 读取视频帧函数：定时器触发的槽函数，用于读取视频帧并进行目标检测
void MainWindow::readFrame()
{
    cv::Mat frame; // 定义帧图像对象
    capture->read(frame); // 读取一帧视频图像
    if (frame.empty()) return; // 如果帧为空，则退出函数

    auto start = std::chrono::steady_clock::now(); // 记录开始时间
    yolov5->detect(frame); // 执行目标检测
    auto end = std::chrono::steady_clock::now(); // 记录结束时间
    std::chrono::duration<double, std::milli> elapsed = end - start; // 计算执行时间
    ui->textEditlog->append(QString("cost_time: %1 ms").arg(elapsed.count())); // 在文本框中显示执行时间
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB); // 将BGR格式转换为RGB格式
    QImage rawImage = QImage((uchar*)(frame.data),frame.cols,frame.rows,frame.step,QImage::Format_RGB888); // 创建Qt图像对象
    // 自适应大小显示图像
    QPixmap pixmap = QPixmap::fromImage(rawImage);
    pixmap = pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label->setPixmap(pixmap); // 在标签上显示图像
}

// 打开文件按钮点击事件处理函数
void MainWindow::on_openfile_clicked()
{
    // 获取用户选择的文件路径
    QString filename = QFileDialog::getOpenFileName(this, QStringLiteral("打开文件"), ".", "*.mp4 *.avi;;*.png *.jpg *.jpeg *.bmp");
    if (!QFile::exists(filename)) { // 如果文件不存在，则返回
        return;
    }
    ui->statusbar->showMessage(filename); // 在状态栏中显示文件路径

    QMimeDatabase db; // 创建MIME类型数据库对象
    QMimeType mime = db.mimeTypeForFile(filename); // 获取文件的MIME类型
    if (mime.name().startsWith("image/")) { // 如果是图像文件
        cv::Mat src = cv::imread(filename.toLatin1().data()); // 读取图像文件
        if (src.empty()) { // 如果图像为空，则显示提示信息并返回
            ui->statusbar->showMessage("图像不存在！");
            return;
        }
        cv::Mat temp; // 创建临时图像对象
        if (src.channels() == 4) // 如果图像有4个通道，则转换为RGB格式
            cv::cvtColor(src, temp, cv::COLOR_BGRA2RGB);
        else if (src.channels() == 3) // 如果图像有3个通道，则转换为RGB格式
            cv::cvtColor(src, temp, cv::COLOR_BGR2RGB);
        else // 否则，保持原样
            cv::cvtColor(src, temp, cv::COLOR_GRAY2RGB);

        auto start = std::chrono::steady_clock::now(); // 记录开始时间
        yolov5->detect(temp); // 对图像进行目标检测
        auto end = std::chrono::steady_clock::now(); // 记录结束时间
        std::chrono::duration<double, std::milli> elapsed = end - start; // 计算执行时间
        ui->textEditlog->append(QString("cost_time: %1 ms").arg(elapsed.count())); // 在文本框中显示执行时间
        QImage img = QImage((uchar*)(temp.data), temp.cols, temp.rows, temp.step, QImage::Format_RGB888); // 创建Qt图像对象
        ui->label->setPixmap(QPixmap::fromImage(img)); // 在标签上显示图像
        ui->label->resize(ui->label->pixmap()->size()); // 调整标签大小以适应图像大小
        filename.clear(); // 清空文件名
    }
    else if (mime.name().startsWith("video/")) { // 如果是视频文件
        capture->open(filename.toLatin1().data()); // 打开视频文件
        if (!capture->isOpened()) { // 如果打开失败，则显示提示信息并返回
            ui->textEditlog->append("fail to open MP4!");
            return;
        }
        IsDetect_ok += 1; // 计数器加1
        if (IsDetect_ok == 2) // 如果计数器等于2，则启用开始检测按钮
            ui->startdetect->setEnabled(true);
        ui->textEditlog->append(QString::fromUtf8("Open video: %1 succesfully!").arg(filename)); // 在文本框中显示成功打开视频的信息

        // 获取视频的基本信息
        long totalFrame = capture->get(cv::CAP_PROP_FRAME_COUNT); // 获取视频总帧数
        int width = capture->get(cv::CAP_PROP_FRAME_WIDTH); // 获取视频宽度
        int height = capture->get(cv::CAP_PROP_FRAME_HEIGHT); // 获取视频高度
        ui->textEditlog->append(QStringLiteral("整个视频共 %1 帧, 宽=%2 高=%3 ").arg(totalFrame).arg(width).arg(height)); // 在文本框中显示视频信息
        ui->label->resize(QSize(width, height)); // 调整标签大小以适应视频大小

        // 设置视频开始帧
        long frameToStart = 0; // 设置起始帧为0
        capture->set(cv::CAP_PROP_POS_FRAMES, frameToStart); // 设置视频的当前帧位置
        ui->textEditlog->append(QStringLiteral("从第 %1 帧开始读").arg(frameToStart)); // 在文本框中显示视频开始帧信息

        // 获取视频帧率
        double rate = capture->get(cv::CAP_PROP_FPS); // 获取视频帧率
        ui->textEditlog->append(QStringLiteral("帧率为: %1 ").arg(rate)); // 在文本框中显示视频帧率信息
    }
}

// 加载模型按钮点击事件处理函数
void MainWindow::on_loadfile_clicked()
{
    // 获取用户选择的模型文件路径
    QString onnxFile = QFileDialog::getOpenFileName(this, QStringLiteral("选择模型"), ".", "*.onnx");
    if (!QFile::exists(onnxFile)) { // 如果文件不存在，则返回
        return;
    }
    ui->statusbar->showMessage(onnxFile); // 在状态栏中显示模型文件路径
    if (!yolov5->loadModel(onnxFile.toLatin1().data())) { // 加载模型文件，如果失败，则在文本框中显示加载失败信息并返回
        ui->textEditlog->append(QStringLiteral("加载模型失败！"));
        return;
    }
    IsDetect_ok += 1; // 计数器加1
    ui->textEditlog->append(QString::fromUtf8("Open onnxFile: %1 succesfully!").arg(onnxFile)); // 在文本框中显示成功加载模型的信息
    if (IsDetect_ok == 2) // 如果计数器等于2，则启用开始检测按钮
        ui->startdetect->setEnabled(true);
}

// 开始检测按钮点击事件处理函数
void MainWindow::on_startdetect_clicked()
{
    timer->start(); // 启动定时器，开始检测
    ui->startdetect->setEnabled(false); // 禁用开始检测按钮
    ui->stopdetect->setEnabled(true); // 启用停止检测按钮
    ui->openfile->setEnabled(false); // 禁用打开文件按钮
    ui->loadfile->setEnabled(false); // 禁用加载模型按钮
    ui->comboBox->setEnabled(false); // 禁用模型选择下拉框
    ui->textEditlog->append(QStringLiteral("=======================\n"
                                           "        开始检测\n"
                                           "=======================\n")); // 在文本框中显示开始检测信息
}

// 停止检测按钮点击事件处理函数
void MainWindow::on_stopdetect_clicked()
{
    ui->startdetect->setEnabled(true); // 启用开始检测按钮
    ui->stopdetect->setEnabled(false); // 禁用停止检测按钮
    ui->openfile->setEnabled(true); // 启用打开文件按钮
    ui->loadfile->setEnabled(true); // 启用加载模型按钮
    ui->comboBox->setEnabled(true); // 启用模型选择下拉框
    timer->stop(); // 停止定时器
    ui->textEditlog->append(QStringLiteral("======================\n"
                                           "        停止检测\n"
                                           "======================\n")); // 在文本框中显示停止检测信息
}

// 模型选择下拉框激活事件处理函数
void MainWindow::on_comboBox_activated(const QString &arg1)
{
    // 根据选择的模型类别设置YOLO网络配置
    if (arg1.contains("s")) { // 如果包含"s"，则选择yolov5s配置
        conf = yolo_nets[0];
    } else if (arg1.contains("m")) { // 如果包含"m"，则选择yolov5m配置
        conf = yolo_nets[1];
    } else if (arg1.contains("l")) { // 如果包含"l"，则选择yolov5l配置
        conf = yolo_nets[2];
    } else if (arg1.contains("x")) { // 如果包含"x"，则选择yolov5x配置
        conf = yolo_nets[3];
    }
    // 初始化YOLOv5对象
    yolov5->Initialization(conf);
    // 在文本框中显示所选择的模型类别及其参数
    ui->textEditlog->append(QStringLiteral("使用模型类别：%1 args: %2 %3 %4")
                            .arg(arg1)
                            .arg(conf.nmsThreshold)
                            .arg(conf.objThreshold)
                            .arg(conf.confThreshold));
}
