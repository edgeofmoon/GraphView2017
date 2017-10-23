#pragma once
#include "mygraphencodingrepresentation.h"

class MyGraphMatrixEncodingRepresentation :
	public MyGraphEncodingRepresentation
{
public:
	MyGraphMatrixEncodingRepresentation(void);
	~MyGraphMatrixEncodingRepresentation(void);
	
	virtual void Show();
	virtual void ShowNode(int i);
	virtual void ShowEdge(int i);
	virtual void ShowHighlightedNode(int i);
	virtual void ShowNodeHighlight(int i);
	virtual void ShowHighlightedEdge(int i);

	virtual void ShowLabel(int idx);
	virtual void ShowLabels();

	virtual int GetNumOfTangibleMarker(int i) const;
	virtual MyVec3f GetNodeTangiblePos(int i, int mIdx = 0) const;
	virtual MyBoundingBox GetNodeTangibleBox(int i, int mIdx = 0) const;

	static const int INSIDE = 0; // 0000
	static const int LEFT = 1;   // 0001
	static const int RIGHT = 2;  // 0010
	static const int BOTTOM = 4; // 0100
	static const int TOP = 8;    // 1000

	virtual void SetLabelDraw(int drawByte);
	virtual int GetLabelDraw() const;

	static const int ROW = 1; // 0001
	static const int COLUMN = 2;   // 0010

	virtual void SetNodeHighlightByte(int byte);
	virtual int GetNodeHighlightByte() const;

	virtual void SetNodeAffectEdgeHighlightByte(int byte);
	virtual int GetNodeAffectEdgeHighlightByte() const;

protected:
	float mCellWidth;
	float mCellHeight;

	float mCellBoarder_X;
	float mCellBoarder_Y;
	
	int mLabelDrawByte;
	int mNodeHighlightByte;
	int mNodeAffectEdgeHighlightByte;

	virtual void DrawRowBox(int startRow, int endRow);
	virtual void DrawColumnBox(int startCol, int endCol);
};

