#include "MyGraphSingleNodeDegreeCountTask.h"


MyGraphSingleNodeDegreeCountTask::MyGraphSingleNodeDegreeCountTask(void)
{
	mTaskDescription = "How many nodes connect to the highlighted one?";
}


MyGraphSingleNodeDegreeCountTask::~MyGraphSingleNodeDegreeCountTask(void)
{
}

void MyGraphSingleNodeDegreeCountTask::ComputeAnswerAndOptions(){
	int numOptions = 3;
	MyArrayi neighbors = this->GetGraph()->GetNeighbors(mNodeIdx);
	int degree = neighbors.size();

	if(neighbors.size() > 0){
		mAnswerIndex = rand()%numOptions;
	}
	else{
		mAnswerIndex = 0;
	}
	mOptions.clear();
	for(int i = 0;i<numOptions;i++){
		int offset = i - mAnswerIndex;
		int option = degree + offset;
		mOptions << MyString(option);
	}
}

MyString MyGraphSingleNodeDegreeCountTask::GetTaskName() const{
	return MyString("Task_Degree_Count");
}
