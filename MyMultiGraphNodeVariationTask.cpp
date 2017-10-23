#include "MyMultiGraphNodeVariationTask.h"
#include "MyGraphAlgorithm.h"
#include "MyGenericHierarchyOverlay.h"

MyMultiGraphNodeVariationTask::MyMultiGraphNodeVariationTask(void)
{
	mTaskDescription = "Task 1: Of which network the highlighted node has higher degree centrality?";
}

MyMultiGraphNodeVariationTask::~MyMultiGraphNodeVariationTask(void)
{
}

void MyMultiGraphNodeVariationTask::ComputeAnswerAndOptions(){
	
	mOptions.clear();
	mOptions << MyString("left")
		<< MyString("right");

	int neighbor0 = this->GetGraph(0)->GetNeighbors(mNodeIdx).size();
	int neighbor1 = this->GetGraph(1)->GetNeighbors(mNodeIdx).size();
	if(neighbor0>neighbor1){
		// left
		mAnswerIndex = 0;
	}
	else if(neighbor0<neighbor1){
		// right
		mAnswerIndex = 1;
	}
	else{
		// same
		mAnswerIndex = -1;
	}
	/*
	for(int i = 0;i<this->GetNumGraphs();i++){
		int nNeighbors = this->GetGraph(i)->GetNeighbors(mNodeIdx).size();
		if(nNeighbors>maxNeighbor){
			maxNeighbor = nNeighbors;
			mAnswerIndex = i;
		}
		mOptions << MyString(i+1);
	}
	*/
}

MyString MyMultiGraphNodeVariationTask::GetTaskName() const{
	return MyString("Task_Node_Variation");
}

int MyMultiGraphNodeVariationTask::InterpretSelectionAnswer(const MyGenericNode* sel) const{
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

MyString MyMultiGraphNodeVariationTask::InterpretSelection(int idx) const{
	MyArrayStr sideArray;
	sideArray << "left" << "right";
	return sideArray[idx];
}