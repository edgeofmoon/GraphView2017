#pragma once
#include "MyGraphChoiceTask.h"

class MyGraphMultiNodeTask
	: public MyGraphChoiceTask
{
public:
	MyGraphMultiNodeTask(void);
	~MyGraphMultiNodeTask(void);

	virtual void SetNodes(int idx1, int idx2);
	virtual void AddNode(int idx);
	virtual void SetNodes(const MyArrayi& idxArray);
	
	virtual void LoadExtraInfo(std::ifstream& infile);
	virtual void AdjustKnot(MyGraphKnot* graphKnot);

protected:
	MyArrayi mNodes;
};

