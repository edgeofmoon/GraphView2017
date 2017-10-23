#pragma once
#include "MyGraphSingleNodeTask.h"

class MyGraphNodeEdgeWeightTask :
	public MyGraphSingleNodeTask
{
public:
	MyGraphNodeEdgeWeightTask(void);
	~MyGraphNodeEdgeWeightTask(void);
	
	virtual void ComputeAnswerAndOptions();
	virtual MyString GetTaskName() const;

	
	virtual bool DoNeedPreHint() const {return true;};
};

