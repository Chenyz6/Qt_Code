#include "Matrix.h"
Matrix::Matrix() {
	rowCount = 1, columnCount = 1;
	r = new char*[1];
	for (int i = 0; i < rowCount; i++)
	{
		r[i] = new char[columnCount];
	}
	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < columnCount; j++)
		{
			r[i][j] = 0;
		}
	}
}
Matrix::Matrix(Matrix& m) {
	rowCount = m.GetRowCount();
	columnCount = m.GetColumnCount();
	r = new char*[rowCount];
	for (int i = 0; i < rowCount; i++)
	{
		r[i] = new char[columnCount];
	}
	for (int i = 0; i < rowCount * columnCount; i++)
	{
		r[i / columnCount][i % columnCount] = m[i / columnCount][i % columnCount];
	}
}
Matrix::Matrix(int row, int column) {
	assert(row > 0 && column > 0);
	rowCount = row, columnCount = column;
	r = new char*[rowCount];
	for (int i = 0; i < rowCount; i++)
	{
		r[i] = new char[columnCount];
	}
	for (int i = 0; i < rowCount * columnCount; i++)
	{
		r[i / columnCount][i % columnCount] = 0;
	}
}
Matrix::Matrix(int row, int column, char* value) {
	assert(row > 0 && column > 0);
	rowCount = row, columnCount = column;
	r = new char*[rowCount];
	for (int i = 0; i < rowCount; i++)
	{
		r[i] = new char[columnCount];
	}
	int count = 0;
	char x = 0;
	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < columnCount; j++)
		{
			r[i][j] = value[count];
			x = value[count];
			count++;
		}
	}
}

int Matrix::GetRowCount() const { return rowCount; }
int Matrix::GetColumnCount() const { return columnCount; }
void Matrix::RowMultiply(int row, double time) {
	if (row < 0 || row >= rowCount) return;
	for (int i = 0; i < columnCount; i++)
	{
		r[row][i] *= time;
	}
}
void Matrix::ColumnMultiply(int column, double time) {
	if (column < 0 || column >= columnCount) return;
	for (int i = 0; i < rowCount; i++)
	{
		r[i][column] *= time;
	}
}
double Matrix::Determinant() {
	if (rowCount != columnCount) return -2147483648.0;
	if (rowCount == 1) return r[0][0];
	if (rowCount == 2) return r[0][0] * r[1][1] - r[0][1] * r[1][0];
	Matrix temp(rowCount,columnCount);
	double res = 1.0;
	for (int i = 0; i < rowCount * columnCount; i++)
	{
		temp[i / columnCount][i%columnCount] = r[i / columnCount][i%columnCount];
	}
	while (true)
	{
		if (temp.GetRowCount() == 2) {
			res *= (temp[0][0] * temp[1][1] - temp[0][1] * temp[1][0]);
			break;
		}
		//找到第一列第一个不为0的数，这个数不变
		int row = 0;
		while (row < temp.GetRowCount() && temp[row][0] == 0) row++;
		if (row == temp.GetRowCount()) { res *= 0; break; }
		//计算到结果中去
		res *= temp[row][0];
		res *= (row % 2 == 0 ? 1 : -1);
		//把该列除了选中的这个数全部置0，同时其他列的数也计算改变,并存为新矩阵
		int a = temp.GetColumnCount() - 1;
		double* arr = new double[a * a];
		int ai = 0;
		for (int i = 0; i < temp.GetRowCount(); i++)
		{
			if(i == row || temp[i][0] == 0) continue; //标识行不变，第一个数也是0，不变
			double coefficience = temp[i][0] / temp[row][0];
			for (int j = 1; j < temp.GetColumnCount(); j++)
			{
				temp[i][j] = temp[i][j] - coefficience * temp[row][j];
			}
			temp[i][0] = 0;
		}
		for (int i = 0; i < temp.GetRowCount(); i++)
		{
			if (i == row) continue; //跳过标识行
			for (int j = 1; j < temp.GetColumnCount(); j++)
			{
				arr[ai++] = temp[i][j];
			}
		}
		//生成新矩阵
		temp.Reshape(a, a);
		for (int i = 0; i < a * a; i++)
		{
			temp[i / a][i % a] = arr[i];
		}
		delete[] arr;
	}

	return res;
}
int Matrix::Rank() const {
	if (rowCount == 1 || columnCount == 1) return 1;
	Matrix temp(rowCount, columnCount);
	for (int i = 0; i < rowCount * columnCount; i++)
	{
		temp[i / columnCount][i % columnCount] = r[i / columnCount][i % columnCount];
	}

	//PrintMatrix(temp);
	int resultrank = 0;
	int c_sum = 0;
	int count = 0;
	while (true)
	{
		//count++;
		// 最后只有一行了，判断该行是否全0
		if (temp.GetRowCount() == 1) {
			bool isAllZero = true;
			for (int i = 0; i < temp.GetColumnCount(); i++)
			{
				if (temp[0][i] != 0) {
					isAllZero = false;
					break;
				}
			}
			resultrank += (isAllZero ? 0 : 1);
			break;
		}
		//如果只有一列了，判断是否全0
		//printf("%d\n", temp.GetColumnCount());
		if (temp.GetColumnCount() == 1) {
			bool isAllZero = true;
			for (int i = 0; i < temp.GetRowCount(); i++)
			{
				if (temp[i][0] != 0) {
					isAllZero = false;
					break;
				}
			}
			resultrank += (isAllZero ? 0 : 1);
			break;
		}
		//找到第一列第一个不为0的数，这个数不变
		int row = 0;
		while (row < temp.GetRowCount() && temp[row][0] == 0) row++;
		//如果这一列全为0,则只需要计算剩下的列的秩
		if (row == temp.GetRowCount()) {
			Matrix p(temp.GetRowCount(), temp.GetColumnCount() - 1);
			for (int i = 0; i < p.GetRowCount(); i++)
			{
				for (int j = 0; j < p.GetColumnCount(); j++)
				{
					p[i][j] = temp[i][j + 1];
				}
			}
			temp.Reshape(p);
			continue;
		}
		//将其他行的第一个数变为0
		for (int i = 0; i < temp.GetRowCount(); i++)
		{
			char num = 0;
			if (i == row || temp[i][0] == 0) continue;
			//printf("%d", temp.r[0][0]);
			for (int j = 1; j < temp.GetColumnCount(); j++)
			{
				//printf("%1f = %1f+%1f",temp[i][j] + temp[row][j], temp[i][j], temp[row][j]);
				num = temp[i][j] + temp[row][j];
				//printf("%f", temp[i][j]);
				//printf("%f", temp[row][j]);
				if ((num == 2) || (num ==0)) temp[i][j] = 0;
				else if (num == 1) temp[i][j] = 1;
				else
				{
					printf("temp error");
					printf("%d,%d", temp[row][j], temp[i][j]);
				}
				//PrintMatrix(temp);
			}
		}
		//PrintMatrix(temp);

		Matrix p(temp.GetRowCount() - 1, temp.GetColumnCount() - 1);

		int curRow = 0, curColumn = 0;
		for (int i = 0; i < temp.GetRowCount(); i++)
		{
			if(i == row) continue;
			for (int j = 1; j < temp.GetColumnCount(); j++)
			{
				p[curRow][curColumn++] = temp[i][j];
			}
			curRow++;
			curColumn = 0;
		}
		//bool a = false;
		
		temp.Reshape(p);
		//PrintMatrix(temp);
		resultrank++;
	}

	return resultrank;
}
//Matrix Matrix::Inverse() {
//    if (rowCount != columnCount)
//    {
//        Matrix m(1, 1, 0);
//        return m;
//    }
//    if(Determinant() == 0)
//    {
//        Matrix m(1, 1, 0);
//        return m;
//    }
//	int n = rowCount;
//	Matrix partner(n, n);
//	for (int i = 0; i < n; i++)
//	{
//		for (int j = 0; j < n; j++)
//		{
//			Matrix p(n - 1, n - 1);
//			int curRow = 0, curColumn = 0;
//			for (int a = 0; a < n; a++)
//			{
//				if (a == i) continue;
//				for (int b = 0; b < n; b++)
//				{
//					if(b == j) continue;
//					p[curRow][curColumn++] = r[a][b];
//				}
//				curRow++;
//				curColumn = 0;
//			}
//			partner[j][i] = p.Determinant() * ((i + j) % 2 == 0 ? 1 : -1);
//		}
//	}
//    return partner * (1.0 / Determinant());
//    // 以上是原文件
//    //有问题 暂时用不上  注释
//}
void Matrix::Transpose() {
	char** p = new char*[columnCount];
	for (int i = 0; i < columnCount; i++)
	{
		p[i] = new char[rowCount];
	}
	for (int i = 0; i < columnCount; i++)
	{
		for (int j = 0; j < rowCount; j++)
		{
			p[i][j] = r[j][i];
		}
	}
	for (int i = 0; i < rowCount; i++)
	{
		delete[] r[i];
	}
	delete[] r;
	r = p;
	int temp = rowCount;
	rowCount = columnCount;
	columnCount = temp;
}

void Matrix::SwapRows(int row1, int row2) {
	if (row1 < 0 || row2 < 0) return;
	if (row2 >= rowCount || row1 >= rowCount) return;
	for (int i = 0; i < columnCount; i++)
	{
		char temp = r[row1][i];
		r[row1][i] = r[row2][i];
		r[row2][i] = temp;
	}
}
void Matrix::SwapColumns(int column1, int column2) {
	if (column1 < 0 || column2 < 0) return;
	if (column1 >= columnCount || column2 >= columnCount) return;
	for (int i = 0; i < rowCount; i++)
	{
		char temp = r[i][column1];
		r[i][column1] = r[i][column2];
		r[i][column2] = temp;
	}
}
void Matrix::Reshape(int row, int column, char value) {
	char** p = new char*[row];
	for (int i = 0; i < row; i++)
	{
		p[i] = new char[column];
	}
	for (int i = 0; i < row * column; i++)
	{
		p[i / column][i%column] = value;
	}
	for (int i = 0; i < rowCount; i++)
	{
		delete[] r[i];
	}
	delete[] r;
	r = p;
	rowCount = row;
	columnCount = column;
}
void Matrix::Reshape(Matrix& m) {
	for (int i = 0; i < rowCount; i++)
	{
		delete[] r[i];
	}
	delete[] r;
	rowCount = m.GetRowCount();
	columnCount = m.GetColumnCount();
	r = new char*[rowCount];
	for (int i = 0; i < rowCount; i++)
	{
		r[i] = new char[columnCount];
	}
	for (int i = 0; i < rowCount * columnCount; i++)
	{
		r[i / columnCount][i % columnCount] = m[i / columnCount][i % columnCount];
	}
	
}
double* Matrix::ToArray()
{
	double* p = new double[rowCount*columnCount];
	for (int i = 0; i < rowCount*columnCount; i++)
	{
		p[i] = r[i / columnCount][i%columnCount];
	}
	return p;
}
Matrix::~Matrix() {
	for (int i = 0; i < rowCount; i++)
	{
		delete[] r[i];
	}
	delete[] r;
}
void Matrix::operator=(Matrix& m)
{
	for (int i = 0; i < rowCount; i++)
	{
		delete[] r[i];
	}
	delete[] r;
	rowCount = m.GetRowCount();
	columnCount = m.GetColumnCount();
	r = new char*[rowCount];
	for (int i = 0; i < rowCount; i++)
	{
		r[i] = new char[columnCount];
	}
	for (int i = 0; i < rowCount * columnCount; i++)
	{
		r[i/ columnCount][i%columnCount] = m[i / columnCount][i%columnCount];
	}

}
char* Matrix::operator[](int row) {
	assert(row < rowCount);		//为真不执行
	return r[row];
}
//Static Member Function
Matrix Matrix::Zeros(int row, int column) {
	assert(row > 0 && column > 0);
    Matrix m(row, column, 0);
    return m;
}
Matrix Matrix::Unit(int sideCount)
{
	assert(sideCount > 0);
	Matrix m(sideCount, sideCount);
	for (int i = 0; i < sideCount; i++)
	{
		m[i][i] = 1;
	}
	return m;
}
//Wild Function
Matrix operator*(Matrix& a, Matrix& b)
{
	assert(a.GetColumnCount() == b.GetRowCount());
	Matrix res(a.GetRowCount(), b.GetColumnCount());
	for (int i = 0; i < a.GetRowCount(); i++)
	{
		for (int j = 0; j < b.GetColumnCount(); j++)
		{
			double sum = 0;
			for (int k = 0; k < a.GetColumnCount(); k++)
			{
				sum += (a[i][k] * b[k][j]);
			}
			res[i][j] = sum;
		}
	}
	return res;
}
Matrix operator+(Matrix& ma, Matrix& mb){ 
    if (ma.GetRowCount() != mb.GetRowCount())
    {
        Matrix m(1, 1, 0);
        return m;
    }
    if (ma.GetColumnCount() != mb.GetColumnCount())
    {
        Matrix m(1, 1, 0);
        return m;
    }
	Matrix res(ma.GetRowCount(), ma.GetColumnCount());
	for (int i = 0; i < ma.GetRowCount(); i++)
	{
		for (int j = 0; j < ma.GetColumnCount(); j++)
		{
			res[i][j] = ma[i][j] + mb[i][j];
		}
	}
	return res; 
}
Matrix operator-(Matrix& ma, Matrix& mb){ 
    if (ma.GetRowCount() != mb.GetRowCount())
    {
        Matrix m(1, 1, 0);
        return m;
    }
    if (ma.GetColumnCount() != mb.GetColumnCount())
    {
        Matrix m(1, 1, 0);
        return m;
    }
	Matrix res(ma.GetRowCount(), ma.GetColumnCount());
	for (int i = 0; i < ma.GetRowCount(); i++)
	{
		for (int j = 0; j < ma.GetColumnCount(); j++)
		{
			res[i][j] = ma[i][j] - mb[i][j];
		}
	}
	return res;
}
void PrintMatrix(Matrix& m) {
	for (int i = 0; i < m.GetRowCount(); i++)
	{
		for (int j = 0; j < m.GetColumnCount(); j++)
		{
			printf("%d ", m[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
