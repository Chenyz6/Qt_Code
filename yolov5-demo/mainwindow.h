#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QFileDialog>
#include <QFile>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QMimeDatabase>
#include <iostream>
#include <yolov5.h>
#include <chrono>


//#pragma comment(lib,"C:\\Program Files (x86)\\Intel\\openvino_2021\\opencv\\lib\\opencv_core453.lib")
//#pragma comment(lib,"C:\\Program Files (x86)\\Intel\\openvino_2021\\opencv\\lib\\opencv_imgcodecs453.lib")
//#pragma comment(lib,"C:\\Program Files (x86)\\Intel\\openvino_2021\\opencv\\lib\\opencv_imgproc453.lib")
//#pragma comment(lib,"C:\\Program Files (x86)\\Intel\\openvino_2021\\opencv\\lib\\opencv_videoio453.lib")
//#pragma comment(lib,"C:\\Program Files (x86)\\Intel\\openvino_2021\\opencv\\lib\\opencv_objdetect453.lib")
//#pragma comment(lib,"C:\\Program Files (x86)\\Intel\\openvino_2021\\opencv\\lib\\opencv_dnn453.lib")
//#pragma comment(lib,"C:\\Program Files (x86)\\Intel\\openvino_2021\\deployment_tools\\inference_engine\\lib\\intel64\\Release\\inference_engine.lib")
//#pragma comment(lib,"C:\\Program Files (x86)\\Intel\\openvino_2021\\deployment_tools\\inference_engine\\lib\\intel64\\Release\\inference_engine_c_api.lib")
//#pragma comment(lib,"C:\\Program Files (x86)\\Intel\\openvino_2021\\deployment_tools\\inference_engine\\lib\\intel64\\Release\\inference_engine_transformations.lib")


//#ifdef QT_NO_DEBUG
//#pragma comment(lib,"D:/opencv_build/install/x64/vc16/lib/opencv_core452.lib")
//#pragma comment(lib,"D:/opencv_build/install/x64/vc16/lib/opencv_imgcodecs452.lib")
//#pragma comment(lib,"D:/opencv_build/install/x64/vc16/lib/opencv_imgproc452.lib")
//#pragma comment(lib,"D:/opencv_build/install/x64/vc16/lib/opencv_imgcodecs452.lib")
//#pragma comment(lib,"D:/opencv_build/install/x64/vc16/lib/opencv_video452.lib")
//#pragma comment(lib,"D:/opencv_build/install/x64/vc16/lib/opencv_videoio452.lib")
//#pragma comment(lib,"D:/opencv_build/install/x64/vc16/lib/opencv_objdetect452.lib")
//#pragma comment(lib,"D:/opencv_build/install/x64/vc16/lib/opencv_shape452.lib")
//#pragma comment(lib,"D:/opencv_build/install/x64/vc16/lib/opencv_dnn452.lib")
//#pragma comment(lib,"D:/opencv_build/install/x64/vc16/lib/opencv_dnn_objdetect452.lib")
//#endif


//#ifdef QT_NO_DEBUG
//#pragma comment(lib,"D:/opencv455_cuda/install/x64/vc16/lib/opencv_core455.lib")
//#pragma comment(lib,"D:/opencv455_cuda/install/x64/vc16/lib/opencv_imgcodecs455.lib")
//#pragma comment(lib,"D:/opencv455_cuda/install/x64/vc16/lib/opencv_imgproc455.lib")
//#pragma comment(lib,"D:/opencv455_cuda/install/x64/vc16/lib/opencv_imgcodecs455.lib")
//#pragma comment(lib,"D:/opencv455_cuda/install/x64/vc16/lib/opencv_video455.lib")
//#pragma comment(lib,"D:/opencv455_cuda/install/x64/vc16/lib/opencv_videoio455.lib")
//#pragma comment(lib,"D:/opencv455_cuda/install/x64/vc16/lib/opencv_objdetect455.lib")
//#pragma comment(lib,"D:/opencv455_cuda/install/x64/vc16/lib/opencv_shape455.lib")
//#pragma comment(lib,"D:/opencv455_cuda/install/x64/vc16/lib/opencv_dnn455.lib")
//#pragma comment(lib,"D:/opencv455_cuda/install/x64/vc16/lib/opencv_dnn_objdetect455.lib")
//#endif



QPixmap Mat2Image(cv::Mat src);

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 主窗口类的定义
class MainWindow : public QMainWindow
{
    Q_OBJECT  // 使用Qt的信号和槽机制必须的宏

public:
    // 构造函数和析构函数
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 初始化函数，通常用于初始化数据和配置
    void Init();

private slots:
    // 定义槽函数，这些函数是响应信号（如按钮点击、时间到达等）的函数
    void readFrame(); // 读取视频帧并处理
    void on_openfile_clicked(); // 处理“打开文件”按钮点击事件
    void on_loadfile_clicked(); // 处理“加载文件”按钮点击事件
    void on_startdetect_clicked(); // 处理“开始检测”按钮点击事件
    void on_stopdetect_clicked(); // 处理“停止检测”按钮点击事件
    void on_comboBox_activated(const QString &arg1); // 处理下拉列表选择改变事件

private:
    // 成员变量
    Ui::MainWindow *ui; // UI界面指针
    QTimer *timer; // 定时器指针，用于定期触发读取视频帧
    cv::VideoCapture *capture; // OpenCV视频捕获对象

    YOLOV5 *yolov5; // YOLOv5对象指针，用于执行目标检测
    NetConfig conf; // 网络配置结构，用于存储网络配置参数
    NetConfig *yolo_nets; // 网络配置数组，用于存储多个网络配置
    std::vector<cv::Rect> bboxes; // 存储检测到的边界框
    int IsDetect_ok = 0; // 检测状态标志，0表示未检测，1表示检测完成
};

#endif // MAINWINDOW_H
