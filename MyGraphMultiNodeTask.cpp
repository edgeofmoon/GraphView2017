#include "MyGraphMultiNodeTask.h"
#include "MyGraphKnot.h"

MyGraphMultiNodeTask::MyGraphMultiNodeTask(void)
{
}


MyGraphMultiNodeTask::~MyGraphMultiNodeTask(void)
{
}

void MyGraphMultiNodeTask::SetNodes(int idx1, int idx2){
	mNodes.clear();
	mNodes << idx1 << idx2;
}

void MyGraphMultiNodeTask::AddNode(int idx){
	mNodes << idx;
}

void MyGraphMultiNodeTask::SetNodes(const MyArrayi& idxArray){
	mNodes = idxArray;
}

void MyGraphMultiNodeTask::LoadExtraInfo(std::ifstream& infile){
	int n;
	infile >> n;
	mNodes.clear();
	for(int i = 0;i<n;i++){
		int idx;
		infile >> idx;
		mNodes << idx;
	}
}

void MyGraphMultiNodeTask::AdjustKnot(MyGraphKnot* graphKnot){
	for(int i = 0;i<mNodes.size();i++){
		MyGenericHierarchyOverlayStr* node = new MyGenericHierarchyOverlayStr(MyString(mNodes[i]));
		graphKnot->AddStaticSelection(node);
	}
	graphKnot->HighlightStaticSelection();
}