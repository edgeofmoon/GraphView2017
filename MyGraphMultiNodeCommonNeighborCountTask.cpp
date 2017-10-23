#include "MyGraphMultiNodeCommonNeighborCountTask.h"
#include "MyGraphKnot.h"

MyGraphMultiNodeCommonNeighborCountTask::MyGraphMultiNodeCommonNeighborCountTask(void)
{
	mNodes << 0 << 1;
	mTaskDescription = "How many common neighbors do the two highlighted nodes have?";
}


MyGraphMultiNodeCommonNeighborCountTask::~MyGraphMultiNodeCommonNeighborCountTask(void)
{
}

void MyGraphMultiNodeCommonNeighborCountTask::ComputeAnswerAndOptions(){
	int numOptions = 3;
	MyArrayi neighbor00 = this->GetGraph(0)->GetNeighbors(mNodes[0]);
	MyArrayi neighbor01 = this->GetGraph(0)->GetNeighbors(mNodes[1]);
	MyArrayi* commomNeighbor = MyArrayi::MakeCommonElementArray(neighbor00, neighbor01);

	int numNeighbor = commomNeighbor->size();
	delete commomNeighbor;

	// make sure no option has negative number
	do{
		int r = rand()*+rand()*3.1415926575123;
		mAnswerIndex = r%numOptions;
	}while(numNeighbor < mAnswerIndex);

	mOptions.clear();
	for(int i = 0;i<numOptions;i++){
		int option = numNeighbor + i-mAnswerIndex;
		mOptions << MyString(option);
	}

}

MyString MyGraphMultiNodeCommonNeighborCountTask::GetTaskName() const{
	return MyString("Task_Common_Neighbors");
}

void MyGraphMultiNodeCommonNeighborCountTask::AdjustKnot(MyGraphKnot* graphKnot){
	MyGraphMultiNodeTask::AdjustKnot(graphKnot);
	graphKnot->GetRepresentation()->SetHightlightNodeAffectEdge(true);
}