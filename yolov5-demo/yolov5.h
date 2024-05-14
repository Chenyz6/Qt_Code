#ifndef YOLOV5_H
#define YOLOV5_H
#include <opencv2/opencv.hpp>       // OpenCV库
#include <opencv2/dnn.hpp>          // OpenCV深度学习模块
#include <opencv2/core/cuda.hpp>    // OpenCV CUDA模块
#include <fstream>      // 文件流
#include <sstream>      // 字符串流
#include <iostream>
#include <exception>    // 异常处理
#include <QMessageBox>


// 结构体，用于存储YOLOv5网络的配置参数
struct NetConfig
{
    float confThreshold; // 类别置信度阈值
    float nmsThreshold;  // 非极大值抑制阈值
    float objThreshold;  // 目标置信度阈值
    std::string netname; // 网络名称
};

// YOLOV5类
class YOLOV5
{
public:
    YOLOV5(){} // 构造函数

    // 初始化方法，接受一个NetConfig结构体参数
    void Initialization(NetConfig conf);

    // 加载模型方法，接受一个ONNX文件路径参数
    bool loadModel(const char* onnxfile);

    // 检测方法，接受一个cv::Mat图像帧参数
    void detect(cv::Mat& frame);

private:
    // 锚点和步长等相关参数
    const float anchors[3][6] = {{10.0, 13.0, 16.0, 30.0, 33.0, 23.0},
                                 {30.0, 61.0, 62.0, 45.0, 59.0, 119.0},
                                 {116.0, 90.0, 156.0, 198.0, 373.0, 326.0}};
    const float stride[3] = { 8.0, 16.0, 32.0 };

    // 类别名称数组
    std::string classes[80] = {"person", "bicycle", "car", "motorbike", "aeroplane", "bus",
                              "train", "truck", "boat", "traffic light", "fire hydrant",
                              "stop sign", "parking meter", "bench", "bird", "cat", "dog",
                              "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe",
                              "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
                              "skis", "snowboard", "sports ball", "kite", "baseball bat",
                              "baseball glove", "skateboard", "surfboard", "tennis racket",
                              "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl",
                              "banana", "apple", "sandwich", "orange", "broccoli", "carrot",
                              "hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant",
                              "bed", "diningtable", "toilet", "tvmonitor", "laptop", "mouse",
                              "remote", "keyboard", "cell phone", "microwave", "oven", "toaster",
                              "sink", "refrigerator", "book", "clock", "vase", "scissors",
                              "teddy bear", "hair drier", "toothbrush"};

    // 输入图像宽高
    const int inpWidth = 640;
    const int inpHeight = 640;

    // 置信度阈值、非极大值抑制阈值、目标置信度阈值
    float confThreshold;
    float nmsThreshold;
    float objThreshold;

    // 用于测试的blob矩阵
    std::vector<int> blob_sizes{ 1, 3, 640, 640};
    cv::Mat blob = cv::Mat(blob_sizes, CV_32FC1, cv::Scalar(0.0));

    // 输出结果相关的变量
    std::vector<cv::Mat> outs;
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    std::vector<int> indices;

    // 神经网络对象 DNN(Deep Neural Networks)
    cv::dnn::Net net;

    // 绘制预测结果方法
    void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame);

    // sigmoid函数应用于Mat矩阵的每个元素
    void sigmoid(cv::Mat* out, int length);
};

// 静态内联函数，用于对x进行sigmoid函数变换
static inline float sigmoid_x(float x)
{
    return static_cast<float>(1.f / (1.f + exp(-x)));
}

#endif // YOLOV5_H
