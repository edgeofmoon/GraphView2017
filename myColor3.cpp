#include "myColor3.h"

myColor3::myColor3()
{
	a = b = c =0;
}
void myColor3::setmyColor(int x, int y, int z)
{
	a=x;
	b=y;
	c=z;
}
myColor3 myColor3::operator /(int mul)
{
	if(mul == 0)
	{
		//			do something here!
	}
	myColor3 temp;
	temp.a = (int)(a / (float)mul + 0.5);
	ABS(temp.a);
	temp.b = (int)(b / (float)mul + 0.5);
	ABS(temp.b);
	temp.c = (int)(c / (float)mul + 0.5);
	ABS(temp.c);
	return temp;
}

int myColor3::operator [](int index)
{
	index = index % 3;
	if(index == 0)
	{
		return a;
	}
	else
	{
		if(index == 1)
		{
			return b;
		}
		else
		{
			if(index == 2)
			{
				return c;
			}
			else
			{
				return c;
			}
		}
	}
}

myColor3 myColor3::operator *(int mul)
{
	myColor3 temp;
	temp.a = a * mul;
	ABS(temp.a);
	temp.b = b * mul;
	ABS(temp.b);
	temp.c = c * mul;
	ABS(temp.c);
	return temp;
}

myColor3 myColor3::operator +(myColor3 add)
{
	myColor3 temp;
	temp.a = a + add[0];
	ABS(temp.a);
	temp.b = b + add[1];
	ABS(temp.b);
	temp.c = c + add[2];
	ABS(temp.c);
	return temp;
}

myColor3 myColor3::operator -(myColor3 add)
{
	myColor3 temp;
	temp.a = a - add[0];
	ABS(temp.a);
	temp.b = b - add[1];
	ABS(temp.b);
	temp.c = c - add[2];
	ABS(temp.c);
	return temp;
}

int myColor3::ABS(int &color)
{
	if(color > 255)
	{
		color = 255;
	}
	if(color < 0)
	{
		color = 0;
	}
	return 0;
}