#pragma once

#include "MyGraphChoiceTask.h"
#include "MyGenericEventListener.h"
#include "MyGraphKnot.h"

class MyGraphTaskUI
	:public MyGenericEventListener
{
public:
	MyGraphTaskUI(void);
	~MyGraphTaskUI(void);

	static int mBackground;

	void SetEnable(bool b);
	void SetTask(MyGraphChoiceTask* task);
	void SetGraphKnot(MyGraphKnot* knot);
	void Show();
	void SetResultShown(bool b = true) {mShowResult = b;};
	int GetHitBoxIndex(int mx, int my, int winWidth, int winHeight) const;
	void SetUserSelection(int idx);
	int GetUserSelection() const;
	float GetUserAnswerValue() const;
	virtual MyVec2f GetAnswerValueRange() const;
	void SetIndex(int i);
	void SetTotalNumTrials(int total){mTotal = total;};
	void DrawLegendTick();
	bool DoNeedTick() const;
	void SetJumpOption(bool b = true);
	bool IsPaused() const;
	void SetFinishBoxString(const MyString& str);

	void SetWarningText(const MyString& str){ mWarningString = str; };
	
	void UpdateUserAnswerFromGraphKnot();

	bool IsAnswerSet() const;

	
	int EventHandler(MyGenericEvent& eve);

protected:
	bool mEnabled;

	int mIdx;
	int mTotal;
	bool mShowResult;
	const MyGraphChoiceTask* mTask;

	MyString mWarningString;
	MyString mFinishBoxString;

	virtual void updateTickValue(int mx, int my, int winWidth, int winHeight);
	virtual int mouseReleaseEventHandler(MyGenericEvent& eve);
	virtual int mouseMoveEventHandler(MyGenericEvent& eve);
	virtual int mousePressEventHandler(MyGenericEvent& eve);
	MyGraphKnot* mGraphKnot;

	int mHighlightedIndex;
	int mUserSelection;
	bool mJumpAllowed;
	bool mFinished;

	float mTickValue;
	bool mTickSet;
	MyBoundingBox mLegendBox;
	bool mBeingDrag;

	bool mIsPaused;
};

