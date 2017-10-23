#include "LabCut.h"

int LabCut::get_Index(int index)
{
	Lab3D point = get_Position(index);
	return (point.L) *101*101 + (point.a) * 101 + (point.b);
}

Lab3D LabCut::get_PositionI(int index)
{
	return get_Position((float)index/(float)Steps);
}

Lab3D LabCut::get_Position(float percentage)
{
	float _shift = PI / 2.0f;
	Lab3D result;
	float x,y,z;
	x = (float)(LH - LL) * percentage + LL;
	y = cos(percentage * PI * 1.6 + _shift) * R;
	z = sin(percentage * PI * 1.6 + _shift) * R;
	result.L = (int)(x + 0.5);
	//result.L = 65;
	result.a = (int)(y + 0.5);
	result.b = (int)(z + 0.5);
	return result;
}

LabCut::LabCut()
{
	R = 60;
	LL = 45;
	LH = 75;
	Steps = 100;
}

LabCut::LabCut(int r, int l1, int l2, int steps)
{
	R = r;
	LL = l1;
	LH = l2;
	Steps = steps;
}