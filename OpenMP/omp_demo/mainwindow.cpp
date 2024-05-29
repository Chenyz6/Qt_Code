#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;

void matrix_multiply(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    int N = A.size();
#pragma omp parallel for collapse(2)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // int N = 1000; // Matrix size
    // vector<vector<int>> A(N, vector<int>(N, 1));
    // vector<vector<int>> B(N, vector<int>(N, 1));
    // vector<vector<int>> C(N, vector<int>(N, 0));

    // auto start = chrono::high_resolution_clock::now();
    // matrix_multiply(A, B, C);
    // auto end = chrono::high_resolution_clock::now();

    // chrono::duration<double> elapsed = end - start;
    // cout << "Time taken with OpenMP: " << elapsed.count() << " seconds" << endl;

    qDebug() << omp_get_max_threads();
    qDebug() << omp_get_num_procs();

#pragma omp parallel for num_threads(8)
    for (int i = 0; i < 16; i++)
    {
        qDebug() << "OpenMP Test, 线程编号为: " << omp_get_thread_num();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
