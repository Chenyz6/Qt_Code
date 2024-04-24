#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <memory>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};


class Solution {
public:
    int integerBreak(int n) {
        vector<int> dp(n + 1, 0);
        dp[2] = 1;
        for(int i = 3; i <= n; i++){
            for(int j = 1; j < i; j++){
                dp[i] = max(dp[i], max((i - j) * j, dp[i - j] * j));
            }
        }
        return dp[n];
    }
};

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::pushButton_clicked()
{

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    chart = new QChart();
    chartView = new QChartView(chart);
//    layout = new QVBoxLayout(ui->widget);
    chartView->setRenderHint(QPainter::Antialiasing);
    // 设置 chartView 为正方形
    int sideLength = 300; // 设置正方形的边长
    chartView->setFixedSize(sideLength, sideLength);
//    ui->widget->setStyleSheet("background-color:rgb(37,37,38)");  // 设置控件背景颜色

    connect(&timer, &QTimer::timeout,this,[=](){
        series = new QLineSeries;
        for(int temp = 0; temp < 2048; temp++)
        {
            series->append(qrand() % 100, qrand() % 200);
        }

        pushButton_clicked();
    });
    timer.start(500);

}

class MainWindow1 {
public:
    bool canPartition(vector<int>& nums) {

    }
};










