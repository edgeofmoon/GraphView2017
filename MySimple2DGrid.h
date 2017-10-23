#pragma once

#include "MyVec.h"
#include "MyArray.h"
#include "MyLine.h"

class MySimple2DGrid
{
public:
	MySimple2DGrid(void);
	~MySimple2DGrid(void);

	void SetRange(float startx, float endx, float starty, float endy);
	void SetRange(const MyVec2f& start, const MyVec2f& end);
	void SetXDivision(int n);
	void SetYDivision(int n);

	MyVec2f GetLowPos() const {return mStart;};
	MyVec2f GetHighPos() const {return mEnd;};
	int GetDivisionX() const {return mDivisionX;};
	int GetDivisionY() const {return mDivisionY;};
	
	virtual int GetStatusByte(MyVec2f pos) const;
	virtual bool IsIndexValid(const MyVec2i& index) const;
	virtual MyVec2i GetGridIndex(MyVec2f pos) const;
	virtual MyVec2i ClampIndex(const MyVec2i& idx) const;
	virtual MyArray2i* MakePassGridIndexArray(MyVec2f start, MyVec2f end) const;
	virtual float GetCellWidth() const;
	virtual float GetCellHeight() const;
	virtual MyVec2f GetCellSize() const;
	virtual MyVec2f GetLowPos(const MyVec2i& idx) const;

	//MyArray2i* MakeCellsAround(const MyVec2f& center, float maxDistance) const;
	MyArray2f* MakeContourArray(const MyArray2i& inArray, bool clockWise = true) const;
	MyArray2i* MakeBoxCollidingCells(const MyVec2f& low, const MyVec2f high) const;

	static bool IsCellConnected(const MyVec2i& idx1, const MyVec2i& idx2, bool diagCounts = false);
	static bool IsCellConnected(const MyVec2i& idx1, const MyArray2i& idxArray, bool diagCounts = false);
	static bool IsCellConnected(const MyArray2i& idxArray1, const MyArray2i& idxArray2, bool diagCounts = false);

	static MyArray2i* MakeContourCellIndexArray(const MyArray2i& inArray, bool diagCounts = false);
	static MyArray2i* MakeContourIndexArray(const MyArray2i& inContourArray, bool clockWise = true);
	static MyArray2i* MakeContourIndexArray(const MyVec2i& idx, int neighborByte = 0, bool clockWise = true);

	static int GetConnectionByte(const MyVec2i& source, const MyVec2i& target);
	static int GetConnectionByte(const MyVec2i& idx, const MyArray2i& idxArray);
protected:
	MyVec2f mStart, mEnd;
	int mDivisionX;
	int mDivisionY;

private:
	bool CohenSutherlandLineClip(const MyVec2f& start, const MyVec2f& end,
		MyVec2f& outStart, MyVec2f& outEnd) const;
	
	static const int INSIDE = 0; // 0000
	static const int LEFT = 1;   // 0001
	static const int RIGHT = 2;  // 0010
	static const int BOTTOM = 4; // 0100
	static const int TOP = 8;    // 1000
};

