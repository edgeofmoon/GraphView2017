#pragma once
#include "mygraphrepresentation.h"
#include "MyArray.h"
#include "MyColor4.h"

class MyGraphLodRepresentation :
	public MyGraphRepresentation
{
public:
	MyGraphLodRepresentation(void);
	~MyGraphLodRepresentation(void);
	
	virtual void Update();
	
	virtual void ShowNode(int i);
	virtual void ShowEdge(int i);
	virtual void ShowHighlightedNode(int i);
	virtual void ShowHighlightedEdge(int i);

	virtual void UpdataLabel();
	virtual void ShowLabels();

	virtual MyGenericNode* MakeSelectionFromNode(int nodeIdx) const;
protected:
	MyArrayStr mCurrentLabels;
	MyArray<const MyGenericNode*>* mNodeOverlays;

	int mHierarchyEncodeLevel;
	MyArray<MyColor4f> mNodeColors;

	virtual void updateLodColors();
	virtual void addToSelectionList(const MyGenericNode* root);
};

