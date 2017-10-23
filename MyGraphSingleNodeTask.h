#pragma once
#include "MyGraphChoiceTask.h"

class MyGraphSingleNodeTask
	: public MyGraphChoiceTask
{
public:
	MyGraphSingleNodeTask(void);
	~MyGraphSingleNodeTask(void);

	virtual void SetNodeOfInterest(int idx);
	
	virtual void LoadExtraInfo(std::ifstream& infile);
	
	virtual void AdjustKnot(MyGraphKnot* graphKnot);
protected:
	int mNodeIdx;
};

