#pragma once
#include "mygraphedgebundler.h"

#include "MySimple2DGrid.h"
class MyGraphGeometryEdgeBundler :
	public MyGraphEdgeBundler
{
public:
	MyGraphGeometryEdgeBundler(int dx = 12, int dy = 12);
	~MyGraphGeometryEdgeBundler(void);

	virtual void Update();
	virtual void ShowDebug() const;

protected:
	int mDivisionX;
	int mDivisionY;

	
	MySimple2DGrid grid;
	MyArray<MyArray2i> mergedGrids;
	MyArrayf gridMainAngles;
	MyArrayi gridWeight; // num edges
	MyArray<MyLine2f> gridLine;
	
	MyArray<MyArrayf> mainAngles;
	MyArray<MyLine2f> meshLines;
	MyArray2f intersections;
	MyArrayi edgeToBeFixed;
};

