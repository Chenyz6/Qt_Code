#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "AIS.h"
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    AIS ais_det;
    string file = "/home/ze/Desktop/Dat/IQ复信号/WV_AIS_102.4kHz_1638400.dat";
    string result;

    ais_det.data_input(file, 2, 1638400);
    ais_det.demodulation(102.4e3);
    result = ais_det.decode();

    // appendStringToTxtFile("/home/ze/Desktop/example.txt", QString::fromStdString(result));
    qDebug() << result.data();
}

void appendStringToTxtFile(const QString &filePath, const QString &str)
{
    QFile file(filePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) // 打开文件，追加模式
    {
        QTextStream out(&file);
        out << str << "\n"; // 写入字符串并换行
        file.close();
    }
    else
    {
        qWarning() << "Failed to open file for appending:" << filePath;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
