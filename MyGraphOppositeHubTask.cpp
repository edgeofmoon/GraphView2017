#include "MyGraphOppositeHubTask.h"
#include "MyGraphAlgorithm.h"


MyGraphOppositeHubTask::MyGraphOppositeHubTask(void)
{
	mTaskDescription = "Task 4: Of all the neighbors of the highlighted region, which one has the highest centrality in the opposite hemisphere?";
}


MyGraphOppositeHubTask::~MyGraphOppositeHubTask(void)
{
}

void MyGraphOppositeHubTask::ComputeAnswerAndOptions(){
	int side = this->getSide(mNodeIdx);

	const MyGenericGraphf* graph = this->GetGraph();
	MyArrayi neighbors = graph->GetNeighbors(mNodeIdx);
	
	MyArrayi oppositeNeighbors;
	for(int i = 0;i<neighbors.size();i++){
		if(this->getSide(neighbors[i]) != side){
			oppositeNeighbors << neighbors[i];
		}
	}
	MyGraphAlgorithm algor;
	algor.SetGraph(graph);
	algor.FloydWarshallWithPathReconstruction();
	MyArrayf betweennessCentrality = algor.GetBetweennessCentrality();

	int hubIdx = neighbors[0];
	float mBt = -1;
	for(int i = 0;i<oppositeNeighbors.size();i++){
		int nodeIdx = oppositeNeighbors[i];
		float bt = betweennessCentrality[nodeIdx];
		if(bt>mBt){
			mBt = bt;
			hubIdx = nodeIdx;
		}
	}

	mAnswerIndex = hubIdx;
	mOptions = *mLabels;
	/*
	int numOptions = neighbors.size();
	if(numOptions >= 4) numOptions = 4;
	mAnswerIndex = rand()%numOptions;

	mOptions.clear();
	MyArrayi candidates = oppositeNeighbors;
	for(int i = oppositeNeighbors.size();candidates.size()<numOptions;i++){
		int neighborIdx = i-oppositeNeighbors.size();
		if(!oppositeNeighbors.HasOne(neighbors[neighborIdx])){
			candidates << neighbors[neighborIdx];
		}
	}
	candidates.EraseOne(hubIdx);

	int candIdx = 0;
	for(int i = 0;i<numOptions;i++){
		if(i == mAnswerIndex){
			mOptions << mLabels->at(hubIdx);
		}
		else{
			mOptions << mLabels->at(candidates[candIdx]);
			candIdx++;
		}
	}
	*/
}

MyString MyGraphOppositeHubTask::GetTaskName() const{
	return "Task_Opposite_Hub";
}

int MyGraphOppositeHubTask::getSide(int nodeIdx) const{
	MyString nodeName = mLabels->at(nodeIdx);
	if(nodeName.back()=='L' || nodeName.back()=='l'){
		return 0;
	}
	else{
		return 1;
	}
}