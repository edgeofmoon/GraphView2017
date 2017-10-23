#pragma once
#include "mygraphsinglenodetask.h"
class MyGraphHubNodeTask :
	public MyGraphSingleNodeTask
{
public:
	MyGraphHubNodeTask(void);
	~MyGraphHubNodeTask(void);
	
	virtual void ComputeAnswerAndOptions();
	virtual void AdjustKnot(MyGraphKnot* graphKnot);
	virtual MyString GetTaskName() const;
	
	virtual bool DoNeedPreHint() const {return true;};
};

