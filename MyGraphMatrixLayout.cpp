#include "MyGraphMatrixLayout.h"
#include "MyGenericHierarchyOverlay.h"
#include "VisConfig.h"
#include <cassert>
#include <algorithm>
using namespace std;

MyGraphMatrixLayout::MyGraphMatrixLayout(void)
{
	mHierarchy = 0;
}


MyGraphMatrixLayout::~MyGraphMatrixLayout(void)
{
}

void MyGraphMatrixLayout::Update(){
	int n = mGraph->GetNumNodes();
	MyVec3f offset(1.f,1.f,0.f);
	MyVec3f lowPos = -n*offset/2;
	this->initPos();
	this->updataBoundingBox();
	// tmp sol
	// for matrix encode rep
	mBoundingBox.expandHigh(n / 2*(MATRIXCELLWIDTH-1) + 40.f, 0);
	mBoundingBox.expandLow(n / 2*(MATRIXCELLWIDTH-1), 0);
	mBoundingBox.expandHigh(n / 2*(MATRIXCELLWIDTH-1)+MARKERSCALE*3+25, 1);
	mBoundingBox.expandLow(n / 2*(MATRIXCELLWIDTH-1), 1);
	mBoundingBox.Translate(lowPos);
	
	mOrientations.resize(n);
	this->orderByHierarchy(mHierarchy,lowPos,offset);
	//mPos = mInPos;
}

// exchange CURRENT Pos
void MyGraphMatrixLayout::ExchangPos(int i, int j){
	MyVec3f tmp = mPos[i];
	mPos[i] = mPos[j];
	mPos[j] = tmp;
}

// set current Pos oldPos nodes= to newPos
// nodes Pos before max(oldPos, newPos)
// or after min(oldPos, newPos) don't change
// if oldPos>newPos, node Pos between (newPos, oldPos)
// upgrade Pos by 1; otherwise downgrade by 1
//void MyGraphMatrixLayout::SetPos(int oldPos, int newPos){
//	if(oldPos == newPos) return;
//	int larger = max(oldPos, newPos);
//	int smaller = min(oldPos, newPos);
//	if(oldPos<newPos){
//		for(int i = smaller+1;i<larger-1;i++){
//			mPos[i] -= MyVec3f(1.f,1.f,0.f);
//		}
//	}
//	else{
//		for(int i = smaller+1;i<larger-1;i++){
//			mPos[i] += MyVec3f(1.f,1.f,0.f);
//		}
//	}
//	mPos[oldPos] = MyVec3f((float)newPos,(float)newPos,0.f);
//}

void MyGraphMatrixLayout::initPos(){
	int n = mGraph->GetNumNodes();
	mPos.resize(n);
	for(int i = 0;i<n;i++){
		mPos[i] = MyVec3f((float)i,(float)i,0.f);
	}
}
void MyGraphMatrixLayout::OrderByHierarchy(const MyGenericNode* hierarchy){
	mHierarchy = hierarchy;
}

const MyGenericNode* MyGraphMatrixLayout::GetHierarchy() const{
	return mHierarchy;
}

void MyGraphMatrixLayout::orderByHierarchy(const MyGenericNode* hierarchy, MyVec3f& pos, const MyVec3f& offset){
	if(!hierarchy) return;
	if(!hierarchy->HasAnyChild()){
		// only leave node counts
		int nodeIdx = dynamic_cast<const MyGenericHierarchyOverlayStr*>(hierarchy)->GetObject().ToInt();
		mPos[nodeIdx] = pos;
		pos += offset;
	}
	else{
		for(int i = 0;i<hierarchy->GetNumChildren();i++){
			const MyGenericNode* child = hierarchy->GetChild(i);
			orderByHierarchy(child,pos, offset);
		}
	}
}