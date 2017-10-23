#pragma once
#include "MyGraphSingleNodeTask.h"
class MyMultiGraphNodeVariationTask :
	public MyGraphSingleNodeTask
{
public:
	MyMultiGraphNodeVariationTask(void);
	~MyMultiGraphNodeVariationTask(void);

	virtual void ComputeAnswerAndOptions();
	virtual MyString GetTaskName() const;
	virtual bool DoNeedPreHint() const {return true;};
	
	virtual int InterpretSelectionAnswer(const MyGenericNode* sel) const;
	virtual MyString InterpretSelection(int idx) const;
};

