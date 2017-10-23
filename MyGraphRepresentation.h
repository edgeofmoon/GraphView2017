#pragma once

#include "MyGraphLayout.h"
#include "MyScale.h"
#include "Label.h"
#include "MyArray.h"
#include "MyGenericNode.h"
#include "MyTextObjectGroup.h"
#include <string>

class MyGraphRepresentation
{
public:
	MyGraphRepresentation(void);
	virtual ~MyGraphRepresentation(void);


	static int mBackground;

	virtual void Show();
	virtual void ShowNode(int i);
	virtual void ShowEdge(int i);
	virtual void ShowHighlightedNode(int i);
	virtual void ShowHighlightedEdge(int i);
	virtual void ShowNodeHighlight(int i);
	virtual void ShowStaticHighlightedNode(int i);
	virtual void ShowStaticHighlightedEdge(int i);
	virtual void ShowNodeCrosshair(int i);
	virtual void ShowBoundingBox();
	virtual void ShowAllCrosshairs();

	virtual float GetNodeSize(int i) const;
	virtual float GetHighlightNodeSize(int i) const;
	virtual float GetEdgeWidth(int i) const;
	virtual float GetHighlightEdgeWidth(int i) const;

	virtual bool DoNodesShow() const;
	virtual bool DoEdgesShow() const;
	virtual bool DoNodeShow(int i) const;
	virtual bool DoEdgeShow(int i) const;
	virtual bool DoNodeHighlight(int i) const;
	virtual bool DoEdgeHighlight(int i) const;
	virtual bool DoNodeStaticHighlight(int i) const;
	virtual bool DoEdgeStaticHighlight(int i) const;
	virtual bool DoBoundingBoxShow() const;

	virtual void SetGraphLayout(const MyGraphLayout* layout);
	virtual const MyGraphLayout* GetGraphLayout() const;
	
	virtual void ReplaceSelection(MyGenericNode* selection);
	virtual const MyGenericNode* GetSelection() const;
	
	virtual void ReplaceStaticSelection(MyGenericNode* selection);
	virtual const MyGenericNode* GetStaticSelection() const;

	virtual void SetNodeLabels(const MyArrayStr* nodeLabels);
	virtual void ShowLabel(int idx);
	virtual void ShowLabels();

	virtual void UpdataLabel();
	virtual void Update();

	virtual MyGenericNode* MakeSelectionFromNode(int nodeIdx) const;
	virtual MyGenericNode* MakeSelectionFromEdge(int edgeIdx) const;
	virtual MyGenericNode* MakeSelectionFromNodes(MyArrayi nodeIdx) const;
	virtual MyGenericNode* MakeSelectionFromEdges(MyArrayi edgeIdx) const;

	virtual void SetHightlightNodeAffectEdge(bool b = true);
	virtual void SetHightlightEdgeAffectNode(bool b = true);

	virtual const MyScale<float, MyColor4f>& GetColorScale() const;

	virtual MyColor4f GetNodeLabelColor(int i) const;
	virtual MyColor4f GetEdgeColor(int i) const;
	virtual MyColor4f GetEdgeDefaultColor() const;
	virtual int GetEdgeSelectionStatus(int i) const;
	virtual float GetEdgeWidthf(int i) const;

	virtual int GetNumOfTangibleMarker(int i) const;
	virtual MyVec3f GetNodeTangiblePos(int i, int mIdx = 0) const;
	virtual MyBoundingBox GetNodeTangibleBox(int i, int mIdx = 0) const;

	virtual void DrawSelectableLabel(int nodeIdx, MyVec3f offset, float scale = 1.f, MyQuarternion rotation=MyQuarternion());

	virtual void SetSelectionLevel(int lvl);
protected:
	const MyGraphLayout* mLayout;
	LabelManager mLabelManager;
	MyTextObjectGroup mLabelObjects;
	MyArray3f mLabelPos;
	MyArrayb mLabelShow;
	bool mLabelBuilt;
	MyMatrixi mFillMatrix;
	const MyArrayStr* mLabels;
	
	MyScale<float, MyColor4f> mColorScheme;
	MyScale<float, float> mEdgeSizeScale;

	virtual void addToSelectionList(const MyGenericNode* root);
	MyGenericNode* mSelection;
	MyArrayi mSelectedNodes;
	MyArray2i mSelectedEdges;
	
	virtual void addToStaticSelectionList(const MyGenericNode* root);
	MyGenericNode* mStaticSelection;
	MyArrayi mStaticSelectedNodes;
	MyArray2i mStaticSelectedEdges;

	bool mHighlightNodeAffectEdge;
	bool mHighlightEdgeAffectNode;

	int mSelectionLevel;
};

