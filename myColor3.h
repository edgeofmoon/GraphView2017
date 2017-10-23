#ifndef MYCOLOR3_H
#define MYCOLOR3_H
//#include <QColor>

class myColor3
{
public:
	myColor3();
	void setmyColor(int x,int y,int z);//2013/3/5
	myColor3 operator +(myColor3 add);
	myColor3 operator -(myColor3 sub);
	myColor3 operator *(int mul);
	myColor3 operator /(int div);
	int operator [](int index);
	int ABS(int &color);
	int a;
	int b;
	int c;
};

#endif