#include "MyGraphHierarchicalLayout.h"
#include "MyDenseGraph.h"
#include "MyGraphLocalOptimizationLayout.h"
#include "MyUtility.h"
#include "MyGenericHierarchyOverlay.h"

#include <cmath>

MyGraphHierarchicalLayout::MyGraphHierarchicalLayout()
{
}


MyGraphHierarchicalLayout::~MyGraphHierarchicalLayout()
{
}

void MyGraphHierarchicalLayout::SetHierarchy(const MyGenericNode* hierarchyRoot){
	mHierarchyRoot = hierarchyRoot;
}

void MyGraphHierarchicalLayout::Update(){
	mPos = *mInPos;
	if(this->mFlat){
		this->flatThings();
	}
	layoutSubHierarchy(mHierarchyRoot, mBoundingBox);
	MyBoundingBox oldBox = mBoundingBox;
	this->updataBoundingBox();
	MyArray3f newPos;
	oldBox.MapPoints(mPos,newPos,mBoundingBox);
	mPos = newPos;
	mBoundingBox = oldBox;
	this->updataBoundingBox();
	if(mEdgeBundler){
		mEdgeBundler->SetGraphLayout(this);
		if(mEdgeBundler && mBundleEdge){
			mEdgeBundler->Update();
		}
	}
}

void MyGraphHierarchicalLayout::layoutSubHierarchy(const MyGenericNode* hierarchy, MyBoundingBox boundingBox){
	if (!hierarchy->HasAnyChild()){
		const MyGenericHierarchyOverlayStr *hierarchyOverlay = 
			dynamic_cast<const MyGenericHierarchyOverlayStr*>(hierarchy);
		int leaveIdx = hierarchyOverlay->GetObject().ToInt();
		mPos[leaveIdx] = boundingBox.GetCenter();
	}
	else{
		int nChildren = hierarchy->GetNumChildren();
		std::vector<int> numChildrenList(nChildren);
		for (int i = 0; i < nChildren; i++){
			numChildrenList[i] = hierarchy->GetChild(i)->GetNumChildren();
		}

		// segment the boundingbox
		// use size to calculate distance matrix
		// and use distance matix as graph to layout using MDS
		MyMatrixf* mat = new MyMatrixf(nChildren, nChildren);
		for (int i = 0; i < nChildren; i++){
			for (int  j = i; j < nChildren; j++){
				if (i != j){
					float distance = sqrtf(numChildrenList[i]-1) + sqrtf(numChildrenList[j]-1)+1;
					mat->At(i, j) = distance;
					mat->At(j, i) = distance;
				}
				else{
					mat->At(i, j) = 1.f;
				}
			}
		}

		MyDenseGraphf* graph = new MyDenseGraphf;
		graph->SetMatrix(mat);

		MyGraphLocalOptimizationLayout* layoutHelper = new MyGraphLocalOptimizationLayout;
		layoutHelper->SetGraph(graph);
		layoutHelper->SetBoundingBox(boundingBox);
		//MyArray3f* initPos = boundingBox.MakeRandomPositions(graph->GetNumNodes());
		//layoutHelper->SetNodePos(initPos);
		MyArray3f* initPos = new MyArray3f;
		initPos->Copy(this->mInPos);
		layoutHelper->SetNodePos(mInPos);
		layoutHelper->Update();
		layoutHelper->AdvanceToFinal();

		//int numLeaves = hierarchy->GetNumLeaves();
		//float averageSizeMeasure = powf(numLeaves,0.33);//sqrtf(numLeaves);
		float averageNodeWidth = 5;//boundingBox.GetWidth()/averageSizeMeasure;
		float averageNodeHeight = 5;//boundingBox.GetHeight()/averageSizeMeasure;
		float averageNodeDepth = 5;//boundingBox.GetDepth()/averageSizeMeasure;
		for (int i = 0; i < nChildren; i++){
			float sizeMeasure = sqrtf(numChildrenList[i]);
			float subBoxWidth = sizeMeasure*averageNodeWidth;
			float subBoxHeight = sizeMeasure*averageNodeHeight;
			float subBoxDepth = sizeMeasure*averageNodeDepth;
			MyBoundingBox box(layoutHelper->GetNodePos(i), subBoxWidth, subBoxHeight, subBoxDepth);
			const MyGenericNode* child = hierarchy->GetChild(i);
			this->layoutSubHierarchy(child, box);
		}
		
		SafeFreeObject(initPos);
		SafeFreeObject(layoutHelper);
		SafeFreeObject(graph);
	}
}
void  MyGraphHierarchicalLayout::flatThings(){
	for(int i = 0;i<mPos.size();i++){
		mPos[i][2] = 0.f;
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
