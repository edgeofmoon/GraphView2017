#include "MyGraphHighestCommonNeighborTask.h"
#include "MyGraphAlgorithm.h"

MyGraphHighestCommonNeighborTask::MyGraphHighestCommonNeighborTask(void)
{
	mTaskDescription = "Which of the common neighbors has the highest centrality?";
}


MyGraphHighestCommonNeighborTask::~MyGraphHighestCommonNeighborTask(void)
{
}

void MyGraphHighestCommonNeighborTask::ComputeAnswerAndOptions(){
	// for two nodes in one graph only
	int numOptions = 3;
	const MyGenericGraphf *graph = this->GetGraph(0);
	MyArrayi neighbor0 = graph->GetNeighbors(mNodes[0]);
	MyArrayi neighbor1 = graph->GetNeighbors(mNodes[1]);
	MyArrayi* commomNeighbor = MyArrayi::MakeCommonElementArray(neighbor0, neighbor1);
	
	MyGraphAlgorithm algor;
	algor.SetGraph(graph);
	MyArrayf evs = algor.GetEigenvectorCentrality();

	MyArrayf cevs;
	for(int i = 0;i<commomNeighbor->size();i++){
		cevs << evs[commomNeighbor->at(i)];
	}
	MyArrayi* sortInfo = cevs.MakeSortResultArray();
	

	// make sure no option has negative number
	mAnswerIndex = rand()%numOptions;
	if(mAnswerIndex>=commomNeighbor->size()){
		mAnswerIndex = commomNeighbor->size()-1;
	}
	
	mOptions.clear();
	if(mAnswerIndex<0){
		mAnswerIndex = 0;
		mOptions << MyString("#noption");
	}
	else{
		for(int i = 0;i<numOptions && i<commomNeighbor->size();i++){
			mOptions << mLabels->at(commomNeighbor->at(sortInfo->at(i)));
		}
		mOptions.Swap(0, mAnswerIndex);
	}

	delete sortInfo;
	delete commomNeighbor;
}

MyString MyGraphHighestCommonNeighborTask::GetTaskName() const{
	return MyString("Task_Highest_CN");
}