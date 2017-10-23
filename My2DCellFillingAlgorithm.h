#pragma once

#include "MySimple2DGrid.h"
#include "MyBox.h"
#include "MyMatrix.h"

class My2DCellFillingAlgorithm
{
public:
	My2DCellFillingAlgorithm(void);
	~My2DCellFillingAlgorithm(void);

	virtual void SetGrid(const MySimple2DGrid& grid);
	virtual const MySimple2DGrid& GetGrid() const;
	virtual const MyMatrixi& GetFillMatrix() const;

	// will fill the grid
	// dont care about collision
	virtual void PutObject(const MyBox2f& box);

	// return false if collide
	virtual bool CanPutObject(const MyBox2f& box) const;

	// return true if success
	// else return falses
	virtual bool TryPutObjectNear(const MyBox2f& box,
		const MyVec2f& anchor, float maxDistance);

	// cell level methods
	virtual void FillCell(const MyVec2i& idx);
	virtual void FillCells(const MyArray2i& idxs);
	virtual MyArray2i GetBoxCollidingCells(const MyBox2f& box) const;

	virtual bool IsCellFilled(const MyVec2i& idx) const;
	virtual bool IsAnyCellFilled(const MyArray2i& cells) const;

	virtual int GetCellStatus(const MyVec2i& idx) const;
protected:
	MySimple2DGrid mGrid;

	void buildFillMatrix();
	MyMatrixi mFillMatrix;

	static const int FILLBIT = 0x01;
};

