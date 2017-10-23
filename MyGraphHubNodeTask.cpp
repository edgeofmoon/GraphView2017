#include "MyGraphHubNodeTask.h"
#include "MyGraphKnot.h"
#include "MyGraphAlgorithm.h"

MyGraphHubNodeTask::MyGraphHubNodeTask(void)
{
	mTaskDescription = "Task 2: Among the neighbors of the highlighted node, which one has the highest centrality?";
}


MyGraphHubNodeTask::~MyGraphHubNodeTask(void)
{
}

void MyGraphHubNodeTask::ComputeAnswerAndOptions(){
	const MyGenericGraphf* graph = mGraphs.front();
	MyArrayi neighbors = graph->GetNeighbors(mNodeIdx);
	MyArrayf neighborWeights;

	/*
	// weight by degree
	for(int i = 0;i<neighbors.size();i++){
		int neighbor = neighbors[i];
		MyArrayi neighborsi = graph->GetNeighbors(neighbor);
		float weightSum = 0;
		for(int i = 0;i<neighborsi.size();i++){
			MyVec2i nodes(neighbor,neighborsi[i]);
			if(neighbor>neighborsi[i]){
				nodes = MyVec2i(neighborsi[i], neighbor);
			}
			int edge = graph->GetEdgeIndex(nodes);
			float weight = graph->GetEdgeWeight(edge);
			weightSum += weight;
		}
		neighborWeights << weightSum;
	}
	*/

	// weight by betweenness centrality
	MyGraphAlgorithm algor;
	algor.SetGraph(graph);
	algor.FloydWarshallWithPathReconstruction();
	MyArrayf bets = algor.GetBetweennessCentrality();
	//MyArrayf bets = algor.GetEigenvectorCentrality();
	for(int i = 0;i<neighbors.size();i++){
		int neighbor = neighbors[i];
		neighborWeights << bets[neighbor];
	}

	MyArrayi* sortInfo = neighborWeights.MakeSortResultArray();
	int hubNodeIdx = neighbors[sortInfo->back()];
	mAnswerIndex = hubNodeIdx;
	mOptions = *mLabels;
	/*
	MyArrayi* neighborIdxSortInfo = neighbors.MakeSortResultArray();

	for(int i = 0;i<neighbors.size();i++){
		int neighborIdxSorted = neighbors[neighborIdxSortInfo->at(i)];
		MyString option = mLabels->at(neighborIdxSorted);
		mOptions << option;
		if(neighborIdxSorted == hubNodeIdx){
			mAnswerIndex = i;
		}
	}
		
	delete neighborIdxSortInfo;
	*/
	delete sortInfo;
}

void MyGraphHubNodeTask::AdjustKnot(MyGraphKnot* graphKnot){
	MyGraphSingleNodeTask::AdjustKnot(graphKnot);
	graphKnot->GetRepresentation()->SetHightlightNodeAffectEdge(true);
}

MyString MyGraphHubNodeTask::GetTaskName() const{
	return "Task_Hub_Node";
}