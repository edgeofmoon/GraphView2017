#include "MyGraphClusterVariationComparetask.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphKnot.h"
#include <cassert>

MyGraphClusterVariationComparetask::MyGraphClusterVariationComparetask(void)
{
	mTaskDescription = "Task 2: Of the two highlighted brain lobes, which one changes more in terms of link number from brain on the left to the brain on the right?";
}


MyGraphClusterVariationComparetask::~MyGraphClusterVariationComparetask(void)
{
}
void MyGraphClusterVariationComparetask::ComputeAnswerAndOptions(){
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
	MyArrayi lobeNumEdge1(mHierarchy->GetNumChildren(), 0);

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

	for (int i = 0; i < this->GetGraph(1)->GetNumEdges(); i++){
		MyVec2i edge = this->GetGraph(1)->GetEdge(i);
		int lobe0 = node2lobe[edge[0]];
		int lobe1 = node2lobe[edge[1]];
		if (lobe0 == lobe1){
			lobeNumEdge1[lobe0] ++;
		}
		else{
			lobeNumEdge1[lobe0] ++;
			lobeNumEdge1[lobe1] ++;
		}
	}

	MyArrayi variations;
	for (int i = 0; i < mHierarchy->GetNumChildren(); i++){
		int change = lobeNumEdge0[i] - lobeNumEdge1[i];
		if(change <0) change = -change;
		variations << change;
	}

	if(variations[mClusterIndices[0]] > variations[mClusterIndices[1]]){
		mAnswerIndex = mClusterIndices[0];
	}
	else{
		mAnswerIndex = mClusterIndices[1];
	}
	
	mOptions.clear();
	for(int i = 0;i<mHierarchy->GetNumChildren();i++){
		mOptions << dynamic_cast<const MyGenericHierarchyOverlayStr*>
			(mHierarchy->GetChild(i))->GetObject();
	}
	
	/*
	int numLobes = mHierarchy->GetNumChildren();
	MyArray<MyArray2i> lobeEdges0(numLobes);
	MyArray<MyArray2i> lobeEdges1(numLobes);

	for(int i = 0;i<this->GetGraph(0)->GetNumEdges();i++){
		MyVec2i edge = this->GetGraph(0)->GetEdge(i);
		int lobe0 = node2lobe[edge[0]];
		int lobe1 = node2lobe[edge[1]];
		if (lobe0 == lobe1){
			lobeEdges0[lobe0] << edge;
		}
		else{
			lobeEdges0[lobe0] << edge;
			lobeEdges0[lobe1] << edge;
		}
	}

	for(int i = 0;i<this->GetGraph(1)->GetNumEdges();i++){
		MyVec2i edge = this->GetGraph(1)->GetEdge(i);
		int lobe0 = node2lobe[edge[0]];
		int lobe1 = node2lobe[edge[1]];
		if (lobe0 == lobe1){
			lobeEdges0[lobe0] << edge;
		}
		else{
			lobeEdges1[lobe0] << edge;
			lobeEdges1[lobe1] << edge;
		}
	}

	MyArrayi linkChange;
	for(int i = 0;i<numLobes;i++){
		MyArray2i *commonEdges = MyArray2i::MakeCommonElementArray(lobeEdges0[i],lobeEdges1[i]);
		int change = (lobeEdges0[i].size()-commonEdges->size())
			+(lobeEdges1[i].size()-commonEdges->size());
		delete commonEdges;
		linkChange << change;
	}

	if(linkChange[mClusterIndices[0]] > linkChange[mClusterIndices[1]]){
		mAnswerIndex = 0;
	}
	else{
		mAnswerIndex = 1;
	}

	mOptions.clear();
	for(int i = 0;i<2;i++){
		mOptions << dynamic_cast<const MyGenericHierarchyOverlayStr*>
			(mHierarchy->GetChild(mClusterIndices[i]))->GetObject();
	}
	*/
}

void MyGraphClusterVariationComparetask::LoadExtraInfo(std::ifstream& infile){
	int n;
	infile >>n;
	mClusterIndices.clear();
	for(int i = 0;i<n;i++){
		int t;
		infile >> t;
		mClusterIndices << t;
	}
}

void MyGraphClusterVariationComparetask::AdjustKnot(MyGraphKnot* graphKnot){
	for(int i = 0;i<mClusterIndices.size();i++){
		graphKnot->GetSelectionMask()->AddStaticHighlight(mClusterIndices[i]);
	}
	graphKnot->SetSelectionLevel(1);
	graphKnot->GetRepresentation()->SetSelectionLevel(1);
}
	
MyString MyGraphClusterVariationComparetask::GetTaskName() const{
	return "Task_variation";
}

int MyGraphClusterVariationComparetask::InterpretSelectionAnswer(const MyGenericNode* sel) const{
	// by default
	// expect the cluster idx as answer
	const MyGenericHierarchyOverlayStr* clusterStr = dynamic_cast<const MyGenericHierarchyOverlayStr*>(mHierarchy);
	if(clusterStr){
		MyArrayStr clusterArray = clusterStr->GetChildrenObjects();
		const MyGenericHierarchyOverlayStr* selStr = dynamic_cast<const MyGenericHierarchyOverlayStr*>(sel);
		if(selStr){
			MyString selName = selStr->GetObject();
			int idx = clusterArray.IndexOf(selName);
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
	}
	return -1;
}

MyString MyGraphClusterVariationComparetask::InterpretSelection(int idx) const{
	const MyGenericHierarchyOverlayStr* clusterStr = dynamic_cast<const MyGenericHierarchyOverlayStr*>(mHierarchy);
	if(clusterStr){
		MyArrayStr clusterArray = clusterStr->GetChildrenObjects();
		return clusterArray[idx];
	}
	return "BadData";
}