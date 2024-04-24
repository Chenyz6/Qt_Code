#ifndef YOLOV5_H
#define YOLOV5_H
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/core/cuda.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>
#include <QMessageBox>


struct NetConfig
{
    float confThreshold; // class Confidence threshold
    float nmsThreshold;  // Non-maximum suppression threshold
    float objThreshold;  //Object Confidence threshold
    std::string netname;
};

class YOLOV5
{
public:
    YOLOV5(){}
    void Initialization(NetConfig conf);
    bool loadModel(const char* onnxfile);
    void detect(cv::Mat& frame);
private:
    const float anchors[3][6] = {{10.0, 13.0, 16.0, 30.0, 33.0, 23.0}, {30.0, 61.0, 62.0, 45.0, 59.0, 119.0},{116.0, 90.0, 156.0, 198.0, 373.0, 326.0}};
    const float stride[3] = { 8.0, 16.0, 32.0 };
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
    const int inpWidth = 640;
    const int inpHeight = 640;
    float confThreshold;
    float nmsThreshold;
    float objThreshold;

    //========= test =========
    std::vector<int> blob_sizes{ 1, 3, 640, 640};
    cv::Mat blob = cv::Mat(blob_sizes, CV_32FC1, cv::Scalar(0.0));

    //========== pro ========
    //cv::Mat blob;
    std::vector<cv::Mat> outs;
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    std::vector<int> indices;
    cv::dnn::Net net;
    void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame);
    void sigmoid(cv::Mat* out, int length);
};

static inline float sigmoid_x(float x)
{
    return static_cast<float>(1.f / (1.f + exp(-x)));
}
#endif // YOLOV5_H
