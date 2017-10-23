#pragma once
#include "MyKnot.h"

#include "MyGenericGraph.h"
#include "MyGraphLayout.h"
#include "MyGraphRepresentation.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphSelectionMask.h"
#include "MyArray.h"

#include <string>

class MyGraphKnot :
	public MyKnot
{
public:
	MyGraphKnot(void);
	virtual ~MyGraphKnot(void);
	
	virtual void Build();
	virtual void SetShowMode(int mode);
	virtual void PreShow();
	virtual void Show();
	virtual void Destory();

	// do the selection based on current viewing
	// use hardware selection
	virtual bool SeeWhat(std::vector<MyVec2i>& see);
	virtual bool SeeAny();
	virtual MyGenericNode* MakeSeeWhat();

	void SetGraph(const MyGenericGraphf* graph);
	void SetHierarchy(const MyGenericNode* hierarchy);
	void SetGraphNodePos(const MyArray3f* pos);
	void SetGraphNodeLabel(const MyArrayStr* labels);
	void SetLayout(MyGraphLayout* layout);
	void SetRepresentation(MyGraphRepresentation* representation);
	void SetSelectionMask(MyGraphSelectionMask* selectionMask);
	virtual void SetSelectionLevel(int i);
	
	MyGenericNode* MakeGraphSelectionBySemantic(const MyGenericNode* region);
	//virtual int EventHandler(const MyGenericEvent& eve);
	
	virtual void HighLightSelection();
	void Highlight(std::vector<MyVec2i>& highlight);
	virtual void HighlightStaticSelection();

	MyGraphRepresentation* GetRepresentation();
	const MyGraphRepresentation* GetRepresentation() const;
	MyGraphLayout* GetLayout();
	const MyGraphLayout* GetLayout() const;
	MyGraphSelectionMask* GetSelectionMask();
	const MyGraphSelectionMask* GetSelectionMask() const;

	const MyGenericGraphf* GetGraph() const;
	const MyGenericNode* GetHierarchy() const;
	const MyArray3f* GetGraphNodePos() const;
	const MyArrayStr* GetGraphNodeLabel() const;


protected:
	virtual int keyPressEventHandler(MyGenericEvent& eve);
	virtual int mousePressEventHandler(MyGenericEvent& eve);
	virtual int mouseReleaseEventHandler(MyGenericEvent& eve);
	virtual int mouseMoveEventHandler(MyGenericEvent& eve);
	virtual int idleEventHandler(MyGenericEvent& eve);
	virtual int windowResizeEventHandler(MyGenericEvent& eve);

	virtual void responseBroadcast(const MyKnot* source, int type);

	int mDisplayList;
	const MyArray3f* mInPos;
	const MyArrayStr* mLabels;
	const MyGenericGraphf* mGraph;
	const MyGenericNode* mHierarchy;
	MyGraphLayout* mLayout;
	MyGraphRepresentation* mRepresentation;
	MyGraphSelectionMask* mSelectionMask;
	bool mSelectionFromMask;
	bool mDrawSelectionMask;
};

