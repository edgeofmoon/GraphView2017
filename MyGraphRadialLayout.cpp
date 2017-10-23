#include "MyGraphRadialLayout.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyUtility.h"
MyGraphRadialLayout::MyGraphRadialLayout(void)
{
	mLayerRadius = 20.f;
	mOverallRadius = 60.f;
	mFixedOverallRadius = true;
}


MyGraphRadialLayout::~MyGraphRadialLayout(void)
{
}

void MyGraphRadialLayout::Update(){
	
	mPos = *mInPos;
	int n = mPos.size();
	mOverallRadius = n*2.2f/MY_PI;
	mLayerRadius = mOverallRadius/3;;
	mParentPos.resize(mGraph->GetNumNodes());
	mOrientations.resize(mPos.size());
	layoutSubHierarchy(mHierarchyRoot, MyBoundingBox(
		MyVec3f(0, 0, 0),
		MyVec3f(mOverallRadius, 2*MY_PI, 0)));
	if(mEdgeBundler){
		mEdgeBundler->SetGraphLayout(this);
		if(mEdgeBundler && mBundleEdge){
			mEdgeBundler->Update();
		}
	}
	this->updataBoundingBox();

	mBoundingBox.expandHigh(25,0);
	mBoundingBox.expandHigh(32,1);
	mBoundingBox.expandLow(30,0);
	mBoundingBox.expandLow(32,1);

	mNodeSizes = MyArrayf(mPos.size(),1);
}


void MyGraphRadialLayout::layoutSubHierarchy(const MyGenericNode* hierarchy, MyBoundingBox boundingBox){
	float rootR = boundingBox.GetLeft();
	float rootS = boundingBox.GetBottom();
	float rootE = boundingBox.GetTop();
	// start from 90 degree
	float rootPhi = (rootS+rootE)/2+MY_PI/2;

	MyVec3f rootPos(rootR*cos(rootPhi), rootR*sin(rootPhi),0);

	if(!hierarchy->HasAnyChild()){
		const MyGenericHierarchyOverlayStr* node = 
			dynamic_cast<const MyGenericHierarchyOverlayStr*>(hierarchy);
		int idx = node->GetObject().ToInt();
		mPos[idx] = rootPos;
		float angle = atan2f(rootPos[1], rootPos[0]) - MY_PI/2;
		mOrientations[idx] = MyQuarternion(angle, 0, 0, 1);
	}

	int numChild = hierarchy->GetNumChildren();
	int numLeaf = hierarchy->GetNumLeaves();
	float radianUnit = (rootE-rootS)/numLeaf;
	float startS = rootS;
	for(int i = 0;i<numChild;i++){
		const MyGenericNode* child = hierarchy->GetChild(i);
		int childLeaves = child->GetNumLeaves();
		int childHeight = child->GetHeight();
		float childRadian = childLeaves*radianUnit;
		float endE = startS + childRadian;
		if(mFixedOverallRadius){
			float layerRadius = boundingBox.GetWidth()/(childHeight+1);
			MyGraphRadialLayout::layoutSubHierarchy(child, MyBoundingBox(
				MyVec3f(rootR+layerRadius, startS, 0),
				MyVec3f(boundingBox.GetRight(), endE, 0)));

		}
		else{
			MyGraphRadialLayout::layoutSubHierarchy(child, MyBoundingBox(
				MyVec3f(rootR+mLayerRadius, startS, 0),
				MyVec3f(rootR+mLayerRadius+childHeight*mLayerRadius, endE, 0)));
		}

		if(!child->HasAnyChild()){
			const MyGenericHierarchyOverlayStr* node = 
				dynamic_cast<const MyGenericHierarchyOverlayStr*>(child);
			int idx = node->GetObject().ToInt();
			mParentPos[idx] = rootPos;
		}
		startS = endE;
	}
}

MyArray3f* MyGraphRadialLayout::MakeCLusterCenterArray(const MyArray<MyArrayi>& clusterSet) const{
	MyArray3f* rst = new MyArray3f;
	
	for(int i = 0;i<clusterSet.size();i++){
		int idx = clusterSet[i].front();
		rst->push_back(mParentPos[idx]);
	}

	return rst;
}

void MyGraphRadialLayout::SetNodeSize(MyArrayf sizes){
	mNodeSizes = sizes;
}