#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QGuiApplication>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

#include <QDebug>
#include <QString>
#include "ACARS.h"
#include "AIS.h"
#include<string>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // AIS ais_det;
    // string file = "/home/ze/Desktop/Dat/IQ复信号/WV_AIS_102.4kHz_1638400.dat";
    // string result;

    // ais_det.data_input(file, 2, 1638400);
    // ais_det.demodulation(102.4e3);
    // result = ais_det.decode();

    // appendStringToTxtFile("./AISexample.txt", QString::fromStdString(result));

    ACARS acars_det;
    string result;
    string file = "C:\\Users\\Administrator\\Dat\\WV_ACARS_32kHz_2048000.dat";
    uint32_t len = 1507200 * 20;
    uint32_t type = 2;
    acars_det.data_input(file, type, len);
    acars_det.demodulation(125e3);
    result = acars_det.decode();
    appendStringToTxtFile("./ACARSexample.txt", QString::fromStdString(result));
}

void MainWindow::appendStringToTxtFile(const QString &filePath, const QString &str)
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
