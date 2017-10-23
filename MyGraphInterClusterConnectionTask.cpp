#include "MyGraphInterClusterConnectionTask.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphKnot.h"
#include <cassert>

MyGraphInterClusterConnectionTask::MyGraphInterClusterConnectionTask(void)
{
	mTaskDescription = "Task 1: Which of the two brains has more links connecting the two highlighted brain lobes?";
		//Of the two brain lobe, which changes more in terms of number of connections from the left dataset to the right dataset?";
}


MyGraphInterClusterConnectionTask::~MyGraphInterClusterConnectionTask(void)
{
}

void MyGraphInterClusterConnectionTask::ComputeAnswerAndOptions(){
	MyArrayi node2cluster(this->GetGraph()->GetNumNodes(),-1);
	for(int i = 0;i<mHierarchy->GetNumChildren();i++){
		MyArrayStr nodeIdxStrs;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodeIdxStrs,mHierarchy->GetChild(i));
		for(int inode = 0;inode<nodeIdxStrs.size();inode++){
			int nodeIdx = nodeIdxStrs[inode].ToInt();
			node2cluster[nodeIdx] = i;
		}
	}
	for (int i = 0; i < node2cluster.size(); i++){
		assert(node2cluster[i] >= 0);
	}

	MyArrayi numLinks(2,0);
	for(int igraph = 0;igraph <2;igraph++){
		const MyGenericGraphf* graph = this->GetGraph(igraph);
		for(int iedge = 0;iedge <graph->GetNumEdges();iedge++){
			MyVec2i edge = graph->GetEdge(iedge);
			MyVec2i edgeCluster(node2cluster[edge[0]],node2cluster[edge[1]]);
			if(this->isSameLink(edgeCluster, mClusterIndices)){
				numLinks[igraph]++;
			}
		}
	}
	
	mOptions.clear();
	mOptions << MyString("left")
			<< MyString("right");
	
	if(numLinks[0] < numLinks[1]){
		mAnswerIndex = 1;
	}
	else if(numLinks[0] > numLinks[1]){
		mAnswerIndex = 0;
	}
	else{
		// cannot be same
		mAnswerIndex = -1;
	}
}

void MyGraphInterClusterConnectionTask::LoadExtraInfo(std::ifstream& infile){
	int two;
	infile >> two;
	infile >> mClusterIndices[0] >> mClusterIndices[1];
}

void MyGraphInterClusterConnectionTask::AdjustKnot(MyGraphKnot* graphKnot){
	graphKnot->GetSelectionMask()->AddStaticHighlight(mClusterIndices[0]);
	graphKnot->GetSelectionMask()->AddStaticHighlight(mClusterIndices[1]);
	graphKnot->SetSelectionLevel(1);
	graphKnot->GetRepresentation()->SetSelectionLevel(1);
}
	
MyString MyGraphInterClusterConnectionTask::GetTaskName() const{
	return "Task_Inter_Lobe";
}

bool MyGraphInterClusterConnectionTask::isSameLink(const MyVec2i& a, const MyVec2i& b) const{
	return (a == b)
		|| (a[1] == b[0] && a[0] == b[1]);
	// return (a[0]*a[1]=b[0]*b[1] && a[0]+a[1]=b[0]+b[1]);
}

int MyGraphInterClusterConnectionTask::InterpretSelectionAnswer(const MyGenericNode* sel) const{
	// by default
	// expect the cluster idx as answer
	MyArrayStr sideArray;
	sideArray << "left" << "right";
	const MyGenericHierarchyOverlayStr* selStr = dynamic_cast<const MyGenericHierarchyOverlayStr*>(sel);
	if(selStr){
		MyString selName = selStr->GetObject();
		int idx = sideArray.IndexOf(selName);
		if(idx >= 0){
			return idx;
		}
	}
	for(int i = 0;i<sel->GetNumChildren();i++){
		int idx = this->InterpretSelectionAnswer(sel->GetChild(i));
		if(idx >= 0){
			return idx;
		}
	}
	return -1;
}

MyString MyGraphInterClusterConnectionTask::InterpretSelection(int idx) const{
	MyArrayStr sideArray;
	sideArray << "left" << "right";
	return sideArray[idx];
}