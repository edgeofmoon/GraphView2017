#pragma once

#include "MyGenericEventListener.h"
#include "MyString.h"
#include "MyArray.h"
#include "MyTimer.h"
#include "MySelectionEvent.h"
//#include "MyScene.h"

#include <fstream>

class MyScene;
class MyView;
class MyGraphChoiceTask;
class MyGraphLayout;
class MyGraphRepresentation;
class MyGraphEdgeBundler;
class MyGenericNode;
class MyGraphTaskUI;

class MyGraphTaskInstance
	:public MyGenericEventListener
{
public:
	MyGraphTaskInstance(int idx, int total = 108);
	~MyGraphTaskInstance(void);

	MyGraphChoiceTask* GetTask(){return mTask;};
	MyGraphTaskUI* GetTaskUI() {return mTaskUI;};
	void SetParticipantIndex(int idx);
	void SetTotalTrialNumber(int tn);

	bool IsEmpty() const { return mIsEmpty; };
	MyTimer& GetTimer(){return mTimer;};

	void Log(std::ostream& out);
	void SetUserAnswerIndex(int answerIdx);
	void SetUserAnswerValue(float answerValue);
	void SetShowMode(int mode);
	void Show();
	void LoadFromFile(const MyString& fn);
	MyGraphChoiceTask* NewTaskByType(const MyString& type);
	MyGraphLayout* NewGraphLayoutByType(const MyString& type);
	MyGraphRepresentation* NewGraphRepresentationByType(const MyString& type);
	MyGraphEdgeBundler* NewGraphEdgeBundlerByType(const MyString& type);

	int EventHandler(MyGenericEvent& eve);

	//void LogSelection(std::ostream& out);
	void LogSelection(std::ostream& out, const MySelectionEvent& eve);
	void LogTrialStartIfHavent(std::ostream& out);
	void LogPauseOrResume(std::ostream& out, bool isPause);

	MyScene* GetScene() {return mScene;};
	MyView* GetView() {return mView;};

	MyString GetVisualElementName() const;
	MyString GetLayoutName() const;
	MyString GetTaskName() const;

	static bool IsAnswerSelectionEvent(const MyGenericEvent& eve);

protected:
	int mIdx;
	int mParticipantIndex;
	int mTotalTrials;

	MyTimer mTimer;

	MyScene* mScene;
	MyView* mView;

	MyString mConfigFileName;
	MyGraphChoiceTask* mTask;
	
	MyArray3f* mPos;
	MyArrayStr* mLabels;
	MyGenericNode* mHierarchy;

	MyGraphTaskUI* mTaskUI;

	int mUserAnswerIndex;
	float mUserAnswerValue;
	MyString mLayoutName;
	MyString mEncodingName;
	float mDensity;
	int mDifficulty;
	int mBackground;

	MyArrayi mLastNodeSelection;
	MyArrayi mLastEdgeSelection;

	MyString mLastSelectoinActionName;
	MyString mLastSelectoinActionValue;

	int mShowMode;

	bool mIsEmpty;

	static int LastRecordedTrialIdx;
};

