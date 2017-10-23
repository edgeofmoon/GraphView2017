#ifndef LABOBJECT_H
#define LABOBJECT_H

#include <iostream>
#include <string>
#include <math.h>
#include "myColor3.h"
#include "Lab3D.h"
using namespace std;


class LabObject
{
public:
	LabObject();
	~LabObject();
	int get_Color(Lab3D index, myColor3 &Color)
	{ 
		Color = color[index.L][index.a + 100][index.b + 100];
		return 0;
	}
	int get_Bit(Lab3D index, int &Bit)
	{ 
		Bit = bit[index.L][index.a + 100][index.b + 100];
		return 0;
	};
	int get_Num_Of_Object()
	{ 
		return 101 * 101 * 101;
	}
	int get_Color_Direct(int L, int a, int b, myColor3 &Color, int &Bit)
	{  
		Color = getColor1(L, a, b, Bit);  
		return 0;
	};
private:
	myColor3 getColor1(int L, int a, int b, int &BIT);
	myColor3 color[101][201][201];
	int bit[101][201][201];
};

#endif