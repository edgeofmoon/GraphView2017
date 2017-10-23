#pragma once
#include "mygraphchoicetask.h"
class MyGraphActiveClusterTask :
	public MyGraphChoiceTask
{
public:
	MyGraphActiveClusterTask(void);
	~MyGraphActiveClusterTask(void);

	virtual void ComputeAnswerAndOptions();
	virtual void LoadExtraInfo(std::ifstream& infile);
	virtual void AdjustKnot(MyGraphKnot* graphKnot);
	
	virtual MyString GetTaskName() const;

protected:
	MyArrayi mClusterIndices;
};

