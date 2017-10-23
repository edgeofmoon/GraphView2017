#include "My2DCellFillingAlgorithm.h"


My2DCellFillingAlgorithm::My2DCellFillingAlgorithm(void)
{
}


My2DCellFillingAlgorithm::~My2DCellFillingAlgorithm(void)
{
}

void My2DCellFillingAlgorithm::SetGrid(const MySimple2DGrid& grid){
	mGrid = grid;
	this->buildFillMatrix();
}

const MySimple2DGrid& My2DCellFillingAlgorithm::GetGrid() const{
	return mGrid;
}

const MyMatrixi& My2DCellFillingAlgorithm::GetFillMatrix() const{
	return mFillMatrix;
}

void My2DCellFillingAlgorithm::buildFillMatrix(){
	int m = mGrid.GetDivisionX();
	int n = mGrid.GetDivisionY();
	mFillMatrix = MyMatrixi(m, n, 0);
}

void My2DCellFillingAlgorithm::PutObject(const MyBox2f& box){
	MyArray2i cells = this->GetBoxCollidingCells(box);
	this->FillCells(cells);
}

bool My2DCellFillingAlgorithm::CanPutObject(const MyBox2f& box) const{
	MyVec2i lowIdx = mGrid.GetGridIndex(box.GetLowPos());
	if(!mGrid.IsIndexValid(lowIdx)){
		return false;
	}
	MyVec2i highIdx = mGrid.GetGridIndex(box.GetHighPos());
	if(!mGrid.IsIndexValid(highIdx)){
		return false;
	}
	MyArray2i cells = this->GetBoxCollidingCells(box);
	return !this->IsAnyCellFilled(cells);
}

bool My2DCellFillingAlgorithm::TryPutObjectNear(const MyBox2f& box,
												const MyVec2f& anchor, float maxDistance){
	if(this->CanPutObject(box)){
		this->PutObject(box);
		return true;
	}
	MyBox2f symBox = 2*anchor-box;
	if(this->CanPutObject(symBox)){
		this->PutObject(symBox);
		return true;
	}
	return false;
}

void My2DCellFillingAlgorithm::FillCell(const MyVec2i& idx){
	mFillMatrix.At(idx[0], idx[1]) |= FILLBIT;
}

void My2DCellFillingAlgorithm::FillCells(const MyArray2i& idxs){
	for(unsigned int i = 0;i<idxs.size();i++){
		this->FillCell(idxs[i]);
	}
}

MyArray2i My2DCellFillingAlgorithm::GetBoxCollidingCells(const MyBox2f& box) const{
	MyArray2i* rst = mGrid.MakeBoxCollidingCells(box.GetLowPos(), box.GetHighPos());
	MyArray2i r = *rst;
	delete rst;
	return r;
}

bool My2DCellFillingAlgorithm::IsCellFilled(const MyVec2i& idx) const{
	int status = this->GetCellStatus(idx);
	return (status & FILLBIT) != 0;
}

bool My2DCellFillingAlgorithm::IsAnyCellFilled(const MyArray2i& cells) const{
	for(unsigned int i = 0;i<cells.size();i++){
		if(this->IsCellFilled(cells[i])){
			return true;
		}
	}
	return false;
}

int My2DCellFillingAlgorithm::GetCellStatus(const MyVec2i& idx) const{
	return mFillMatrix.At(idx[0], idx[1]);
}