#include "yolov5.h"
using namespace std;
using namespace cv;

// YOLOV5类的初始化方法，用于设置模型的阈值和预留容器的大小
void YOLOV5::Initialization(NetConfig conf)
{
    // 设置阈值参数
    this->confThreshold = conf.confThreshold; // 类别置信度阈值
    this->nmsThreshold = conf.nmsThreshold;   // 非极大值抑制阈值
    this->objThreshold = conf.objThreshold;   // 目标置信度阈值

    // 预留容器的空间，避免动态分配过程中的性能开销
    classIds.reserve(20);       // 存储类别ID
    confidences.reserve(20);    // 存储类别置信度
    boxes.reserve(20);          // 存储目标框
    outs.reserve(3);            // 存储模型输出
    indices.reserve(20);        // 存储非极大值抑制后的索引
}

// YOLOV5类的模型加载方法，从ONNX文件中加载模型并设置推理后端
bool YOLOV5::loadModel(const char *onnxfile)
{
    try {
        // 从ONNX文件中读取模型
        this->net = cv::dnn::readNetFromONNX(onnxfile);

        // 检查CUDA设备是否可用
        int device_no = cv::cuda::getCudaEnabledDeviceCount();
        if (device_no == 1){
            // 如果有一个以上的CUDA设备可用，将推理后端设置为CUDA
            this->net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
            this->net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        } else {
            // 否则，在CPU上执行推理
            QMessageBox::information(NULL, "warning", QStringLiteral("正在使用CPU推理！\n"), QMessageBox::Yes, QMessageBox::Yes);
        }
        return true; // 模型加载成功
    } catch (exception& e) {
        // 捕获异常，显示错误消息
        QMessageBox::critical(NULL, "Error", QStringLiteral("模型加载出错，请检查重试！\n %1").arg(e.what()), QMessageBox::Yes, QMessageBox::Yes);
        return false; // 模型加载失败
    }
}

// YOLOV5类的目标检测方法，接受一帧图像作为输入并执行目标检测
void YOLOV5::detect(cv::Mat &frame)
{
    // 将图像转换为blob格式以供网络输入
    cv::dnn::blobFromImage(frame, blob, 1 / 255.0, Size(this->inpWidth, this->inpHeight), Scalar(0, 0, 0), true, false);
    this->net.setInput(blob); // 将blob设置为网络的输入
    this->net.forward(outs, this->net.getUnconnectedOutLayersNames()); // 执行前向传播得到输出

    // 生成检测结果
    classIds.clear();     // 清空类别ID
    confidences.clear();  // 清空类别置信度
    boxes.clear();        // 清空目标框
    float ratioh = (float)frame.rows / this->inpHeight, ratiow = (float)frame.cols / this->inpWidth; // 计算缩放比例
    int n = 0, q = 0, i = 0, j = 0, nout = 8 + 5, c = 0;
    for (n = 0; n < 3; n++)   // 遍历三个尺度
    {
        int num_grid_x = (int)(this->inpWidth / this->stride[n]);    // 计算网格数量
        int num_grid_y = (int)(this->inpHeight / this->stride[n]);
        int area = num_grid_x * num_grid_y;
        this->sigmoid(&outs[n], 3 * nout * area);    // 对输出进行sigmoid函数变换
        for (q = 0; q < 3; q++)    // 遍历每个尺度的三个anchor
        {
            const float anchor_w = this->anchors[n][q * 2]; // 获取anchor的宽度和高度
            const float anchor_h = this->anchors[n][q * 2 + 1];
            float* pdata = (float*)outs[n].data + q * nout * area; // 获取对应尺度和anchor的输出数据指针
            for (i = 0; i < num_grid_y; i++) // 遍历网格
            {
                for (j = 0; j < num_grid_x; j++)
                {
                    float box_score = pdata[4 * area + i * num_grid_x + j]; // 目标置信度
                    if (box_score > this->objThreshold) // 若目标置信度超过阈值
                    {
                        float max_class_socre = 0, class_socre = 0;
                        int max_class_id = 0;
                        for (c = 0; c < 80; c++) // 获取类别置信度最大的类别ID
                        {
                            class_socre = pdata[(c + 5) * area + i * num_grid_x + j];
                            if (class_socre > max_class_socre)
                            {
                                max_class_socre = class_socre;
                                max_class_id = c;
                            }
                        }
                        if (max_class_socre > this->confThreshold) // 若类别置信度超过阈值
                        {
                            // 计算目标框的坐标
                            float cx = (pdata[i * num_grid_x + j] * 2.f - 0.5f + j) * this->stride[n];  // 中心点x
                            float cy = (pdata[area + i * num_grid_x + j] * 2.f - 0.5f + i) * this->stride[n]; // 中心点y
                            float w = powf(pdata[2 * area + i * num_grid_x + j] * 2.f, 2.f) * anchor_w;  // 宽度
                            float h = powf(pdata[3 * area + i * num_grid_x + j] * 2.f, 2.f) * anchor_h;  // 高度
                            int left = (cx - 0.5*w)*ratiow; // 左上角坐标
                            int top = (cy - 0.5*h)*ratioh;
                            // 将结果存储到容器中
                            classIds.push_back(max_class_id);
                            confidences.push_back(max_class_socre);
                            boxes.push_back(Rect(left, top, (int)(w*ratiow), (int)(h*ratioh)));
                        }
                    }
                }
            }
        }
    }

    // 执行非极大值抑制，消除重叠框
    indices.clear();
    cv::dnn::NMSBoxes(boxes, confidences, this->confThreshold, this->nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        // 绘制预测结果
        this->drawPred(classIds[idx], confidences[idx], box.x, box.y,
            box.x + box.width, box.y + box.height, frame);
    }
}

// 绘制预测结果
void YOLOV5::drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat &frame)
{
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 0, 255), 3); // 绘制目标框
    string label = format("%.2f", conf); // 格式化置信度字符串
    label = this->classes[classId] + ":" + label; // 添加类别标签
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine); // 获取标签大小
    top = max(top, labelSize.height);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 255, 0), 1); // 绘制标签
}

// 对Mat类型的输出进行sigmoid变换
void YOLOV5::sigmoid(Mat *out, int length)
{
    float* pdata = (float*)(out->data);
    int i = 0;
    for (i = 0; i < length; i++)
    {
        pdata[i] = 1.0 / (1 + expf(-pdata[i])); // sigmoid函数变换
    }
}
