#pragma once
#include "mygraphchoicetask.h"
class MyGraphInterClusterConnectionTask :
	public MyGraphChoiceTask
{
public:
	MyGraphInterClusterConnectionTask(void);
	~MyGraphInterClusterConnectionTask(void);

	virtual void ComputeAnswerAndOptions();
	virtual void LoadExtraInfo(std::ifstream& infile);
	virtual void AdjustKnot(MyGraphKnot* graphKnot);
	
	virtual MyString GetTaskName() const;

	
	virtual int GetAnswerSelectionLevel() const {return 1;};

	virtual int InterpretSelectionAnswer(const MyGenericNode* sel) const;
	virtual MyString InterpretSelection(int idx) const;

protected:
	MyVec2i mClusterIndices;

	bool isSameLink(const MyVec2i& a, const MyVec2i& b) const;
};

