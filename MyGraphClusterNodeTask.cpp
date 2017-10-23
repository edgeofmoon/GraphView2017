#include "MyGraphClusterNodeTask.h"
#include "MyGraphAlgorithm.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphKnot.h"

MyGraphClusterNodeTask::MyGraphClusterNodeTask(void)
{
	mTaskDescription = "Task 3: Of all the regions in the highlighted brain lobe, which one has the highest centrality?";
}


MyGraphClusterNodeTask::~MyGraphClusterNodeTask(void)
{
}

void MyGraphClusterNodeTask::ComputeAnswerAndOptions(){
	const MyGenericGraphf* graph = mGraphs.front();
	MyGraphAlgorithm algor;
	algor.SetGraph(graph);
	algor.FloydWarshallWithPathReconstruction();
	MyArrayf betweennessCentrality = algor.GetBetweennessCentrality();

	const MyGenericNode* cluster = mHierarchy->GetChild(mClusterIndex);
	MyArrayStr idxStrs;
	MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&idxStrs,cluster);

	MyArrayf btns;
	for(int i = 0;i<idxStrs.size();i++){
		int idx = idxStrs[i].ToInt();
		float btc = betweennessCentrality[idx];
		btns << btc;
	}

	MyArrayi* sort = btns.MakeSortResultArray();
	mAnswerIndex = idxStrs[sort->back()].ToInt();
	mOptions = *mLabels;
	/*
	int numOptions = 4;

	mAnswerIndex = rand()%numOptions;

	for(int i = 0;i<numOptions && i<sort->size();i++){
		int nodeIdx = idxStrs[sort->at(sort->size()-1-i)].ToInt();
		mOptions << mLabels->at(nodeIdx);
	}
	mOptions.Swap(mAnswerIndex,0);
	*/
	/*
	mOptions.clear();
	idxStrs.Permute(rand());
	for(int i = 0;i<idxStrs.size();i++){
		int idx = idxStrs[i].ToInt();
		if(i == mAnswerIndex){
			mOptions << mLabels->at(nodeIdx);
		}
		else if(idx != nodeIdx){
			mOptions << mLabels->at(idx);
		}
		if(mOptions.size() >= numOptions){
			break;
		}
	}
	*/
	delete sort;
}

void MyGraphClusterNodeTask::LoadExtraInfo(std::ifstream& infile){
	infile >> mClusterIndex;
}

void MyGraphClusterNodeTask::AdjustKnot(MyGraphKnot* graphKnot){
	graphKnot->SetSelectionLevel(1);
	graphKnot->GetRepresentation()->SetSelectionLevel(1);
	graphKnot->GetSelectionMask()->AddStaticHighlight(mClusterIndex);
}
	
MyString MyGraphClusterNodeTask::GetTaskName() const{
	return "Task_Lobe_Hub";
}