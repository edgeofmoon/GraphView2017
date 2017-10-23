#include "MyGraphLodLayout.h"
#include "MyGraphHierarchicalLayout.h"
#include "MyGraphFilter.h"
#include "MyGenericHierarchyOverlay.h"

MyGraphLodLayout::MyGraphLodLayout(void)
{
}


MyGraphLodLayout::~MyGraphLodLayout(void)
{
	for(int i = 0;i<mGraphs.size();i++){
		delete mGraphs[i];
		delete mPoses[i];
	}
	mGraphs.clear();
	mPoses.clear();
}

void MyGraphLodLayout::SetLevelOfDetailIndex(int i){
	mLodIndex = i;
}
int MyGraphLodLayout::GetLevelOfDetailIndex() const{
	return mLodIndex;
}

int MyGraphLodLayout::GetNumLevelOfDetials() const{
	return mGraphs.size();
}

int MyGraphLodLayout::mDepthToAdd = 0;

MyArray<const MyGenericNode*>* MyGraphLodLayout::mArray = 0;

void MyGraphLodLayout::AddDepthToArray(const MyGenericNode* node){
	int nodeDepth = node->GetDepth();
	if(nodeDepth == MyGraphLodLayout::mDepthToAdd
		|| (!node->HasAnyChild() && nodeDepth<MyGraphLodLayout::mDepthToAdd)){
		MyGraphLodLayout::mArray->push_back(node);
	}
}

MyArray<const MyGenericNode*>* MyGraphLodLayout::MakeHierarchyOverlay() const{
	
	MyGraphLodLayout::mArray = new MyArray<const MyGenericNode*>;
	MyGraphLodLayout::mDepthToAdd = mLodIndex;
	mHierarchyRoot->TraverseDescendants(MyGraphLodLayout::AddDepthToArray);
	return mArray;
}

void MyGraphLodLayout::Update(){
	if(!mPoses.empty()){
		if(mLodIndex<=mPoses.size()-1 && mLodIndex>=0){
			mPos = *(mPoses[mLodIndex]);
		}
		else{
			mPos = *(mPoses.back());
		}
	}
	this->updataBoundingBox();
}

const MyGenericGraphf* MyGraphLodLayout::GetGraph() const{
	if(mLodIndex<=mPoses.size()-1 && mLodIndex>=0){
		return mGraphs[mLodIndex];
	}
	else{
		return mGraphs.back();
	}
}

void MyGraphLodLayout::BuildLodByHierarchy(const MyGenericNode* hierarchyRoot){
	mHierarchyRoot = hierarchyRoot;
	for(int i = 0;i<mGraphs.size();i++){
		delete mGraphs[i];
		delete mPoses[i];
	}
	mGraphs.clear();
	mPoses.clear();


	MyGraphFilter* filter = new MyGraphFilter;
	filter->SetInputGraph(mGraph);
	MyArray<MyGenericGraphf*>* graphsLod = filter->MakeGraphsByHierarchy(hierarchyRoot, &mNodesLod);
	delete filter;
	int numLod = graphsLod->size();
	for(int i = 0;i<numLod;i++){
		mPoses.push_back(new MyArray3f);
		mGraphs.push_back(graphsLod->at(i));
	}
	delete graphsLod;

	//MyGraphHierarchicalLayout* layoutHelper = new MyGraphHierarchicalLayout;
	MyGraphLayout* layoutHelper = new MyGraphLayout;
	layoutHelper->SetGraph(mGraph);
	//layoutHelper->SetHierarchy(hierarchyRoot);
	layoutHelper->SetBoundingBox(mBoundingBox);
	layoutHelper->SetFlat();
	layoutHelper->SetNodePos(mInPos);
	layoutHelper->Update();

	for(int i = 0;i<numLod;i++){
		const MyArray<MyArrayi>& clusters = mNodesLod.at(i);
		for(int iCluster = 0;iCluster < mGraphs[i]->GetNumNodes();iCluster++){
			MyVec3f iClusterCenter(0.f,0.f,0.f);
			for(int inode = 0;inode<clusters[iCluster].size();inode++){
				iClusterCenter+= layoutHelper->GetNodePos(clusters[iCluster][inode]);
			}
			iClusterCenter /= clusters[iCluster].size();
			mPoses[i]->push_back(iClusterCenter);
		}
	}
}