#include "MyGraphChoiceTask.h"
#include "MyGenericHierarchyOverlay.h"

MyGraphChoiceTask::MyGraphChoiceTask(void)
{
	mAnswerIndex = -1;
	mAnswerValue = -1;
}


MyGraphChoiceTask::~MyGraphChoiceTask(void)
{
}

void MyGraphChoiceTask::SetGraph(const MyGenericGraphf* graph){
	mGraphs.clear();
	mGraphs << graph;
}

void MyGraphChoiceTask::SetGraphArray(const MyArray<const MyGenericGraphf*>& graphArray){
	mGraphs = graphArray;
}

void MyGraphChoiceTask::AddGraph(const MyGenericGraphf* graph){
	mGraphs << graph;
}

void MyGraphChoiceTask::SetHierarchy(const MyGenericNode* hierarchy){
	mHierarchy = hierarchy;
}

void MyGraphChoiceTask::SetLabels(const MyArrayStr* labels){
	mLabels = labels;
}

int MyGraphChoiceTask::GetNumOptions() const{
	return mOptions.size();
}

int MyGraphChoiceTask::GetAnswerIndex() const{
	return mAnswerIndex;
}

float MyGraphChoiceTask::GetAnswerValue() const{
	return mAnswerValue;
}

const MyArrayStr& MyGraphChoiceTask::GetOptions() const{
	return mOptions;
}

MyString MyGraphChoiceTask::GetOption(int idx) const{
	return mOptions[idx];
}

MyString MyGraphChoiceTask::GetAnswerString() const{
	if(mOptions.empty()){
		return MyString(mAnswerValue);
	}
	else{
		return mOptions[mAnswerIndex];
	}
}

const MyGenericGraphf* MyGraphChoiceTask::GetGraph(int idx) const{
	return mGraphs[idx];
}

int MyGraphChoiceTask::GetNumGraphs() const{
	return mGraphs.size();
}

void MyGraphChoiceTask::LoadExtraInfo(std::ifstream& infile){
}

void MyGraphChoiceTask::AdjustKnot(MyGraphKnot* graphKnot){
}

MyString MyGraphChoiceTask::GetTaskName() const{
	return MyString("Task_Default");
}

MyString MyGraphChoiceTask::GetTaskDescription() const{
	return mTaskDescription;
}

int MyGraphChoiceTask::InterpretSelectionAnswer(const MyGenericNode* sel) const{
	// by default
	// expect the node idx as answer
	MyGenericHierarchyOverlayStr node("node");
	if(MyGenericHierarchyOverlayStr::IsObjectEqual(sel,&node)){
		for(int i = 0;i<sel->GetNumChildren();i++){
			const MyGenericHierarchyOverlayStr* idxStr = 
				dynamic_cast<const MyGenericHierarchyOverlayStr*>(sel->GetChild(i));
			if(idxStr){
				int idx = idxStr->GetObject().ToInt();
				if(idx>=0){
					return idx;
				}
			}
		}
	}
	else{
		for(int i = 0;i<sel->GetNumChildren();i++){
			int rst = this->InterpretSelectionAnswer(sel->GetChild(i));
			if(rst >= 0){
				return rst;
			}
		}
	}
	return -1;
}

MyString MyGraphChoiceTask::InterpretSelection(int idx) const{
	return mLabels->at(idx);
}