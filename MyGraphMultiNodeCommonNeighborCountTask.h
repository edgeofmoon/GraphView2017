#pragma once
#include "mygraphmultinodetask.h"
class MyGraphMultiNodeCommonNeighborCountTask :
	public MyGraphMultiNodeTask
{
public:
	MyGraphMultiNodeCommonNeighborCountTask(void);
	~MyGraphMultiNodeCommonNeighborCountTask(void);
	
	virtual void ComputeAnswerAndOptions();
	virtual MyString GetTaskName() const;

	virtual void AdjustKnot(MyGraphKnot* graphKnot);
};

