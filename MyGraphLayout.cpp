#include "MyGraphLayout.h"
#include "VisConfig.h"
#include <cmath>

MyGraphLayout::MyGraphLayout(void)
{
	mFlat = false;
	mBundleEdge = false;
	mEdgeBundler = 0;
}


MyGraphLayout::~MyGraphLayout(void)
{
	delete mEdgeBundler;
}

void MyGraphLayout::SetGraph(const MyGenericGraphf* graph){
	mGraph = graph;
}

void MyGraphLayout::SetEdgeBundler(MyGraphEdgeBundler* edgeBundler){
	mEdgeBundler = edgeBundler;
}

void MyGraphLayout::SetEdgeBundle(bool bundle){
	mBundleEdge = bundle;
}

bool MyGraphLayout::GetEdgeBundle() const{
	return mBundleEdge;
}

void MyGraphLayout::Update(){
	switch(mUse){
	case NO_USE:
	case USE_AS_INIT:
	case USE_AS_FINAL:
	default:
		mPos = *mInPos;
		mOrientations.resize(mPos.size());
		break;
	}
	if(mFlat){
		this->flatThings();
	}
	
	this->updataBoundingBox();

	// let 0 0 0 be center
	MyVec3f center = mBoundingBox.GetCenter();
	for(int i = 0;i<mPos.size();i++){
		mPos[i] -= center;
	}
	this->updataBoundingBox();
	mBoundingBox.expandHigh(25,0);
	mBoundingBox.expandLow(25,0);
	mBoundingBox.expandHigh(8,1);
	mBoundingBox.expandLow(8,1);
	if(mEdgeBundler){
		mEdgeBundler->SetGraphLayout(this);
		if(mEdgeBundler && mBundleEdge){
			mEdgeBundler->Update();
		}
	}
}

const MyGenericGraphf* MyGraphLayout::GetGraph() const{
	return mGraph;
}

const MyGraphEdgeBundler* MyGraphLayout::GetEdgeBundler() const{
	return mEdgeBundler;
}

void MyGraphLayout::SetFlat(bool f){
	mFlat = f;
}

void MyGraphLayout::flatThings(){
	for(int i = 0;i<mPos.size();i++){
		MyVec3f pos = mPos[i];
		if(DATASET == 0){
			// for 74 dataset
			//mPos[i][0] = pos[1];
			//mPos[i][1] = pos[2];
			mPos[i][2] = 0.f;
		}
		else if(DATASET == 1){
			// for 167 dataset
			mPos[i][1] = pos[2];
			mPos[i][2] = pos[1];
			// so called flat
			mPos[i][2] = 0;
		}
	}
	// can not be exactly the same
	for(int i = 0;i<mPos.size();i++){
		for(int j = i+1;j<mGraph->GetNumNodes();j++){
			MyVec3f deltaPos = mPos[i]-mPos[j];
			if(deltaPos == MyVec3f::zero()){
				mPos[i][0]+=0.05;
			}
		}
	}

	mBoundingBox.SquashDimension(2);
}

MyArray3f* MyGraphLayout::MakeCLusterCenterArray(const MyArray<MyArrayi>& clusterSet) const{
	MyArray3f* clusterCenter = new MyArray3f;
	for(int iCluster = 0;iCluster<clusterSet.size();iCluster++){
		MyVec3f center(0.f,0.f,0.f);
		for(int iNode = 0;iNode<clusterSet[iCluster].size();iNode++){
			int node = clusterSet[iCluster][iNode];
			center += mPos.at(node);
		}
		if(clusterSet[iCluster].size()>0){
			center /= clusterSet[iCluster].size();
		}
		clusterCenter->push_back(center);
	}
	return clusterCenter;
}