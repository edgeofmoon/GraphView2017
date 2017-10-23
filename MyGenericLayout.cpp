#include "MyGenericLayout.h"
#include "MyGenericLayoutSyncer.h"

MyGenericLayout::MyGenericLayout(void)
{
	mSyncer = 0;
}


MyGenericLayout::~MyGenericLayout(void)
{
}

void MyGenericLayout::SetNodePos(const MyArray3f* pos){
	mInPos = pos;
}

void MyGenericLayout::Update(){
	this->updataBoundingBox();
}

void MyGenericLayout::SetNodePosUse(PosHint use){
	mUse = use;
}

MyVec3f MyGenericLayout::GetNodePos(int i) const{
	return mPos[i];
}

MyQuarternion MyGenericLayout::GetNodeOrientation(int i) const{
	return mOrientations[i];
}

int MyGenericLayout::GetNodeAtPos(const MyVec3f& pos) const{
	return mPos.IndexOf(pos);
}

MyBoundingBox MyGenericLayout::GetBoundingBox() const{
	return mBoundingBox;
}

void MyGenericLayout::updataBoundingBox(){
	mBoundingBox.Reset();
	for(int i = 0;i<mPos.size();i++){
		mBoundingBox.Engulf(mPos[i]);
	}
}

void MyGenericLayout::SetBoundingBox(const MyBoundingBox& box){
	mBoundingBox = box;
}

void MyGenericLayout::MapToBoundingBox(const MyBoundingBox& box){
	this->updataBoundingBox();
	MyArray3f newPos;
	box.MapPoints(mPos,newPos,mBoundingBox);
	mPos = newPos;
	mBoundingBox = box;
}

void MyGenericLayout::SetSyncer(MyGenericLayoutSyncer* syncer){
	mSyncer = syncer;
}

MyGenericLayoutSyncer* MyGenericLayout::GetSyncer() const{
	return mSyncer;
}

void MyGenericLayout::SyncUpdate(){
	if(!mSyncer){
		this->Update();
	}
	else{
		mSyncer->Update(this);
	}
}