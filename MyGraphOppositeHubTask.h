#pragma once
#include "mygraphsinglenodetask.h"
class MyGraphOppositeHubTask :
	public MyGraphSingleNodeTask
{
public:
	MyGraphOppositeHubTask(void);
	~MyGraphOppositeHubTask(void);
	
	virtual void ComputeAnswerAndOptions();
	//virtual void AdjustKnot(MyGraphKnot* graphKnot);
	virtual MyString GetTaskName() const;
	
	virtual bool DoNeedPreHint() const {return true;};

protected:
	// 0~left
	// 1~right
	int getSide(int nodeIdx) const;

};

