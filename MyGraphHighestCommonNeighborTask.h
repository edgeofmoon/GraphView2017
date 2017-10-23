#pragma once
#include "mygraphmultinodetask.h"

class MyGraphHighestCommonNeighborTask :
	public MyGraphMultiNodeTask
{
public:
	MyGraphHighestCommonNeighborTask(void);
	~MyGraphHighestCommonNeighborTask(void);

	virtual void ComputeAnswerAndOptions();
	virtual MyString GetTaskName() const;
};

