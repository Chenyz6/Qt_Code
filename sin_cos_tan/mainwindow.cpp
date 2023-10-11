#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    double center_x = -10;
    double center_y = -10;   // (10,10)
    double end_x = 0;
    double end_y = 0;
    double angle = 45 + 90 + 90 + 90;   // 角度
    double Weights = 1.414; // 权值

    if(0 <= angle && angle < 90){
        end_x = center_x + Weights * sin(angle * PI/180);
        end_y = center_y + Weights * cos(angle * PI/180);
    }else if(90 <= angle && angle < 180){
        double temp_angle = angle;
        temp_angle -= 90;
        end_x = center_x + Weights * sin(temp_angle * PI/180);
        end_y = center_y - Weights * cos(temp_angle * PI/180);
    }else if(180 <= angle && angle < 270){
        double temp_angle = angle;
        temp_angle -= 180;
        end_x = center_x - Weights * sin(temp_angle * PI/180);
        end_y = center_y - Weights * cos(temp_angle * PI/180);
    }else if(270 <= angle && angle < 360){
        double temp_angle = angle;
        temp_angle -= 270;
        end_x = center_x - Weights * sin(temp_angle * PI/180);
        end_y = center_y + Weights * cos(temp_angle * PI/180);
    }

    qDebug() << "";

    qDebug() << "(center_x,center_y) = (" << center_y << "," << center_y << ")";
    qDebug() << "(end_x,end_y) = (" << end_x << "," << end_y << ")";



    close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

