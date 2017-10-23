#pragma once
#include "mygraphsinglenodetask.h"
class MyGraphSingleNodeDegreeCountTask :
	public MyGraphSingleNodeTask
{
public:
	MyGraphSingleNodeDegreeCountTask(void);
	~MyGraphSingleNodeDegreeCountTask(void);

	
	virtual void ComputeAnswerAndOptions();
	virtual MyString GetTaskName() const;
};

