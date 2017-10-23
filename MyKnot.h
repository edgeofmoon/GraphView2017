#pragma once

#include "MyGenericNode.h"
#include "MyGenericKnot.h"
#include "MyGenericEventListener.h"

#include "MyArray.h"

class MyKnot
	:public MyGenericNode,
	public MyGenericKnot,
	public MyGenericEventListener
{
public:
	MyKnot(MyKnot* parent = 0);
	virtual ~MyKnot(void);

	static const int BROADCAST_TYPE_NONE = 0;
	static const int BROADCAST_TYPE_HIGHLIGHT_SELECTION = 1;

	virtual void BroadCastMessage(int type = MyKnot::BROADCAST_TYPE_NONE, bool responseFirst = true);

	virtual void HighLightSelection(){};
	virtual void HighlightStaticSelection(){};

	virtual void SetSelectionLevel(int i);
	virtual int GetSelectionLevel() const;
	virtual void ReplaceSelection(MyGenericNode* selection);
	virtual void AddSelection(MyGenericNode* selection);
	virtual void RemoveSelection(const MyGenericNode* selection);
	virtual bool DoIncludeSelection(const MyGenericNode* selection) const;
	virtual const MyGenericNode* GetSelection(int i) const;
	virtual void ClearSelections();
	virtual int GetNumSelections() const;

	virtual void AddStaticSelection(MyGenericNode* selection);

protected:
	virtual void conveyBroadcast(const MyKnot* source, int msgIdx, int type, bool responseFirst);
	virtual void responseBroadcast(const MyKnot* source, int type);

	virtual void replaceSelections(const MyKnot* source);

	int mMessageIndex;
	static int mGlobalMessageIndex;

	// this selection might contains things that
	// this knot does not have
	MyArray<MyGenericNode*> mSelections;

	// something not user changeable
	MyArray<MyGenericNode*> mStaticSelections;

	int mSelectionLevel;
};

