#pragma once

#include "MyGenericGraph.h"
#include "MyGenericNode.h"
#include "MyArray.h"
#include "MyString.h"

#include <fstream>

class MyGraphKnot;

class MyGraphChoiceTask
{
public:
	MyGraphChoiceTask(void);
	~MyGraphChoiceTask(void);

	virtual int GetNumOptions() const;
	virtual int GetAnswerIndex() const;
	virtual float GetAnswerValue() const;
	virtual const MyArrayStr& GetOptions() const;
	virtual MyString GetOption(int idx) const;
	virtual MyString GetAnswerString() const;
	virtual const MyGenericGraphf* GetGraph(int idx = 0) const;
	virtual int GetNumGraphs() const;
	virtual void ComputeAnswerAndOptions() = 0;
	virtual void LoadExtraInfo(std::ifstream& infile);
	virtual void AdjustKnot(MyGraphKnot* graphKnot);

	virtual void SetGraph(const MyGenericGraphf* graph);
	virtual void SetGraphArray(const MyArray<const MyGenericGraphf*>& graphArray);
	virtual void AddGraph(const MyGenericGraphf* graph);

	virtual void SetHierarchy(const MyGenericNode* hierarchy);
	virtual void SetLabels(const MyArrayStr* labels);

	virtual MyString GetTaskName() const;
	virtual MyString GetTaskDescription() const;

	virtual bool DoNeedPreHint() const {return false;};

	virtual int GetAnswerSelectionLevel() const {return 0;};

	virtual int InterpretSelectionAnswer(const MyGenericNode* sel) const;
	virtual MyString InterpretSelection(int idx) const;

protected:
	MyArray<const MyGenericGraphf*> mGraphs;
	const MyGenericNode* mHierarchy;
	const MyArrayStr* mLabels;

	MyArrayStr mOptions;
	int mAnswerIndex;
	float mAnswerValue;

	MyString mTaskDescription;
};

