#ifndef LABCUT_H
#define LABCUT_H
#include <math.h>
#include "Lab3D.h"

#define PI 3.1415926

class LabCut
{
public:
	LabCut();
	LabCut(int R, int LL, int LH, int steps);
	Lab3D get_Position(float percentage);
	Lab3D get_PositionI(int index);
	int get_Index(int index);
	int get_Steps(){ return Steps; }
private:
	int R;
	int LL, LH;
	int Steps;
};

#endif