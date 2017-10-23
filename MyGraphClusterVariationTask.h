#pragma once
#include "mygraphchoicetask.h"
class MyGraphClusterVariationTask :
	public MyGraphChoiceTask
{
public:
	MyGraphClusterVariationTask(void);
	~MyGraphClusterVariationTask(void);
	
	virtual void ComputeAnswerAndOptions();
	virtual void LoadExtraInfo(std::ifstream& infile);
	virtual void AdjustKnot(MyGraphKnot* graphKnot);
	
	virtual MyString GetTaskName() const;
protected:
	float mLeastDifferenceRatio;
};

