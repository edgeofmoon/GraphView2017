#include "MyGraphNodeEdgeWeightTask.h"
#include "MyGraphAlgorithm.h"

MyGraphNodeEdgeWeightTask::MyGraphNodeEdgeWeightTask(void)
{
	mTaskDescription = "Task 1: What is the degree centrality of the highlighted node?";
}


MyGraphNodeEdgeWeightTask::~MyGraphNodeEdgeWeightTask(void)
{
}

void MyGraphNodeEdgeWeightTask::ComputeAnswerAndOptions(){
	
	//const MyGenericGraphf* graph = mGraphs.front();
	//MyGraphAlgorithm algor;
	//algor.SetGraph(graph);
	//MyArrayf degreeCentrality = algor.GetDegreeCentrality();
	//float weightSum = degreeCentrality[mNodeIdx];
	///*
	//MyArrayi neighbors = graph->GetNeighbors(mNodeIdx);
	//float weightSum = 0;
	//for(int i = 0;i<neighbors.size();i++){
	//	MyVec2i nodes(mNodeIdx,neighbors[i]);
	//	if(mNodeIdx>neighbors[i]){
	//		nodes = MyVec2i(neighbors[i], mNodeIdx);
	//	}
	//	int edge = graph->GetEdgeIndex(nodes);
	//	float weight = graph->GetEdgeWeight(edge);
	//	weightSum += weight;
	//}
	//*/
	//mAnswerIndex = rand()%3;
	//float variance = 0.3;
	//for(int i = 0;i<3;i++){
	//	float ratio = (i-mAnswerIndex)*variance+1.0f;
	//	MyString option(weightSum*ratio);
	//	mOptions << option;
	//}
	//mOptions.clear();
	
	MyArrayi neighbors = this->GetGraph()->GetNeighbors(mNodeIdx);
	int degree = neighbors.size();
	int numNode = this->GetGraph()->GetNumNodes();
	mAnswerValue = degree/(float)(numNode-1);
}

MyString MyGraphNodeEdgeWeightTask::GetTaskName() const{
	return MyString("Task_Degree_Judge");
}
