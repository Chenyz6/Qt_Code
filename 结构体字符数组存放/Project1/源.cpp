#include <iostream>
using namespace std;
#include <string>
struct student 
{
	int i;
	string str;
};
int main()
{
	student s,s2;
	s.i = 10;
	s.str = "123";
	char* cData = new char[sizeof(student)];
	memcpy(cData, &s, sizeof(student));



	memcpy(&s2, cData, sizeof(student));
	system("pause");
	return 0;
}