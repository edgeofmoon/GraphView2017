#pragma once

#include "MyGraphRepresentation.h"
#include "MyGenericNode.h"
#include "MyTextObjectGroup.h"

class MyGraphSelectionMask
{
public:

	static int mBackground;

	MyGraphSelectionMask(void);
	~MyGraphSelectionMask(void);

	void SetGraphRepresentation(const MyGraphRepresentation* rep);
	void SetHierarchy(const MyGenericNode* hierarchy);

	MyGenericNode* MakeSelection(int i) const;

	virtual void Update();

	virtual void Show();

	virtual void AddStaticHighlight(int i);

protected:
	const MyGraphRepresentation* mRepresentation;
	const MyGenericNode* mHierarchy;

	MyArray<MyArray<MyBoundingBox>> mClusterBox;

	MyArray3f mMaskerLabelPos;
	MyArrayi mMaskerLabelAlignment;

	MyArrayi mStaticHighlight;

	MyTextObjectGroup mLabelObjects;
};

