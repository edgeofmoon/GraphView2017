#pragma once
#include "mygraphchoicetask.h"
class MyGraphClusterVariationComparetask :
	public MyGraphChoiceTask
{
public:
	MyGraphClusterVariationComparetask(void);
	~MyGraphClusterVariationComparetask(void);
	
	virtual void ComputeAnswerAndOptions();
	virtual void LoadExtraInfo(std::ifstream& infile);
	virtual void AdjustKnot(MyGraphKnot* graphKnot);
	
	virtual MyString GetTaskName() const;

	
	virtual int GetAnswerSelectionLevel() const {return 1;};

	virtual int InterpretSelectionAnswer(const MyGenericNode* sel) const;
	virtual MyString InterpretSelection(int idx) const;

protected:
	MyArrayi mClusterIndices;
};

