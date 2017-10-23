#pragma once
#include "mygraphchoicetask.h"
class MyGraphClusterNodeTask :
	public MyGraphChoiceTask
{
public:
	MyGraphClusterNodeTask(void);
	~MyGraphClusterNodeTask(void);
	
	virtual void ComputeAnswerAndOptions();
	virtual void LoadExtraInfo(std::ifstream& infile);
	virtual void AdjustKnot(MyGraphKnot* graphKnot);
	
	virtual MyString GetTaskName() const;

protected:
	int mClusterIndex;
};

