#ifndef MATRIX_H
#define MATRIX_H
#include <string>
#include <cassert>
#include <iostream>
#include <typeinfo>
using namespace std;
class Matrix
{
public:
	Matrix();
	Matrix(Matrix& m);
	Matrix(int row, int column);
	Matrix(int row, int column, char* value);
	static Matrix Zeros(int row, int column);
	static Matrix Unit(int sideCount);
    int GetRowCount() const;
    int GetColumnCount() const;
	void RowMultiply(int row, double time);
	void ColumnMultiply(int column, double time);
//	Matrix Inverse();
	double Determinant();
    int Rank() const;
	void Transpose();
	void SwapRows(int row1, int row2);
	void SwapColumns(int column1, int column2);
	void Reshape(int row, int column, char value = 0);
	void Reshape(Matrix& m);
	double* ToArray();
	~Matrix();
	void operator=(Matrix& m);
	//可以确保行检查，不能保证列检查
	char* operator[](int row);
private:
	char** r;
	int rowCount, columnCount;
};

Matrix operator*(Matrix& a, Matrix& b);
template<typename T>
Matrix operator*(T a, Matrix b) {
	assert(typeid(a) == typeid(int) || typeid(a) == typeid(float) || typeid(a) == typeid(double));
	for (int i = 0; i < b.GetRowCount() * b.GetColumnCount(); i++)
	{
		b[i / b.GetRowCount()][i % b.GetColumnCount()] *= a;
	}
	return b;
}
template<typename T>
Matrix operator*(Matrix a, T b) {
	assert(typeid(b) == typeid(int) || typeid(b) == typeid(float) || typeid(b) == typeid(double));
	for (int i = 0; i < a.GetRowCount() * a.GetColumnCount(); i++)
	{
		a[i / a.GetRowCount()][i % a.GetColumnCount()] *= b;
	}
	return a;
}
Matrix operator+(Matrix& ma, Matrix& mb);
Matrix operator-(Matrix& ma, Matrix& mb);
void PrintMatrix(Matrix& m);

#endif

