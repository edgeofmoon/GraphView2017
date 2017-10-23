#include "MyGraphActiveClusterTask.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphKnot.h"
#include <cassert>

MyGraphActiveClusterTask::MyGraphActiveClusterTask(void)
{
	mTaskDescription = "Task 7: Of all lobe on the left brain, which one has the most number of links?";
}


MyGraphActiveClusterTask::~MyGraphActiveClusterTask(void)
{
}

void MyGraphActiveClusterTask::ComputeAnswerAndOptions(){
	MyArrayi node2lobe(this->GetGraph()->GetNumNodes(), -1);
	for (int i = 0; i<mHierarchy->GetNumChildren(); i++){
		const MyGenericNode* child = mHierarchy->GetChild(i);
		MyArrayStr nodesStr;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodesStr, child);
		for (int j = 0; j < nodesStr.size(); j++){
			int node = nodesStr[j].ToInt();
			node2lobe[node] = i;
		}
	}

	for (int i = 0; i < node2lobe.size(); i++){
		assert(node2lobe[i] >= 0);
	}

	
	// this is purely link count change
	// not number of links change
	MyArrayi lobeNumEdge0(mHierarchy->GetNumChildren(), 0);

	for (int i = 0; i < this->GetGraph(0)->GetNumEdges(); i++){
		MyVec2i edge = this->GetGraph(0)->GetEdge(i);
		int lobe0 = node2lobe[edge[0]];
		int lobe1 = node2lobe[edge[1]];
		if (lobe0 == lobe1){
			lobeNumEdge0[lobe0] ++;
		}
		else{
			lobeNumEdge0[lobe0] ++;
			lobeNumEdge0[lobe1] ++;
		}
	}

	MyArrayi candidateLobeEdges;
	for(int i = 0;i<mClusterIndices.size();i++){
		candidateLobeEdges << lobeNumEdge0[mClusterIndices[i]];
	}

	int maxCandidateIdx = candidateLobeEdges.GetMaximumIndex();
	int lobeIdx = mClusterIndices[maxCandidateIdx];

	mAnswerIndex = maxCandidateIdx;
	mOptions.clear();
	for(int i = 0;i<mClusterIndices.size();i++){
		mOptions << dynamic_cast<const MyGenericHierarchyOverlayStr*>
			(mHierarchy->GetChild(mClusterIndices[i]))->GetObject();
	}

	if(mOptions[0].back() == 'R' ||mOptions[0].back() == 'r'){
		mTaskDescription = "Task 8: Of all lobe on the right brain, which one has the most number of links?";
	}
}

void MyGraphActiveClusterTask::LoadExtraInfo(std::ifstream& infile){
	int n;
	infile >>n;
	mClusterIndices.clear();
	for(int i = 0;i<n;i++){
		int t;
		infile >> t;
		mClusterIndices << t;
	}
}

void MyGraphActiveClusterTask::AdjustKnot(MyGraphKnot* graphKnot){
	for(int i = 0;i<mClusterIndices.size();i++){
		graphKnot->GetSelectionMask()->AddStaticHighlight(mClusterIndices[i]);
	}
	graphKnot->SetSelectionLevel(1);
	graphKnot->GetRepresentation()->SetSelectionLevel(1);
}
	
MyString MyGraphActiveClusterTask::GetTaskName() const{
	return "Task_Active_Lobe";
}