#include "MyGraphClusterVariationTask.h"
#include "MyGraphAlgorithm.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphKnot.h"
MyGraphClusterVariationTask::MyGraphClusterVariationTask(void)
{
	mLeastDifferenceRatio = 0.2;
	//mTaskDescription = "Task 3: Among the 8 brain areas, which one shows the largest change from brain on the left to the the brain on the right?";
	mTaskDescription = "Task 5: Among the 8 brain areas, which one has the most number of links change from brain on the left to the the brain on the right?";
}


MyGraphClusterVariationTask::~MyGraphClusterVariationTask(void)
{
}

void MyGraphClusterVariationTask::LoadExtraInfo(std::ifstream& infile){
	int zero;
	infile >> zero;
}

void MyGraphClusterVariationTask::ComputeAnswerAndOptions(){
	/*
	MyGraphAlgorithm algor;
	algor.SetGraph(this->GetGraph(0));
	MyArrayf dcs_0 = algor.GetDegreeCentrality();
	algor.SetGraph(this->GetGraph(1));
	MyArrayf dcs_1 = algor.GetDegreeCentrality();
	*/
	/*
	// this is for the question
	// Of which graph the highlight lobe has more total degree
	const MyGenericHierarchyOverlayStr* cluster 
		= dynamic_cast<const MyGenericHierarchyOverlayStr*>(mHierarchy->GetChild(mClusterIndex));
	MyArrayStr segstr;
	MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&segstr,cluster);
	MyArrayi seg;
	for(int j = 0;j<segstr.size();j++){
		seg << segstr[j].ToInt();
	}

	float dct_0 = 0;
	float dct_1 = 0;
	for(int i = 0;i<seg.size();i++){
		dct_0 += dcs_0[seg[i]];
		dct_1 += dcs_1[seg[i]];
	}

	if(dct_0 >= dct_1*(1+mLeastDifferenceRatio)){
		mAnswerIndex = 0;
	}
	else if(dct_0 <= dct_1*(1-mLeastDifferenceRatio)){
		mAnswerIndex = 1;
	}
	else{
		mAnswerIndex = 2;
	}

	mOptions	<< "Left"
				<< "Right"
				<< "Close/Same";
	*/
	/*
	// this is for question
	// Among the 8 brain areas, which one shows the largest (ratio) total centrality 
	// change from brain on the left to the the brain on the right?
	mOptions.clear();
	MyArray<MyArrayi> segs;
	for(int i = 0;i<mHierarchy->GetNumChildren();i++){
		const MyGenericHierarchyOverlayStr* cluster 
			= dynamic_cast<const MyGenericHierarchyOverlayStr*>(mHierarchy->GetChild(i));
		MyArrayStr segstr;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&segstr,cluster);
		MyArrayi seg;
		for(int j = 0;j<segstr.size();j++){
			seg << segstr[j].ToInt();
		}
		segs << seg;
		mOptions << cluster->GetObject();
	}

	MyArrayf dcDiff;
	for(int i = 0;i<segs.size();i++){
		float dct_0 = 0;
		float dct_1 = 0;
		for(int j = 0;j<segs[i].size();j++){
			dct_0 += dcs_0[segs[i][j]];
			dct_1 += dcs_1[segs[i][j]];
		}
		dcDiff << fabs(dct_0-dct_1)/dct_0;
	}

	int idx = dcDiff.GetMaximumIndex();
	mAnswerIndex = idx;
	*/

	// this is for question
	// which lobe has most link number change from left to right graph?
	MyArrayi node2lobe(this->GetGraph()->GetNumNodes(), -1);
	for (int i = 0; i<mHierarchy->GetNumChildren(); i++){
		const MyGenericNode* child = mHierarchy->GetChild(i);
		MyArrayStr nodesStr;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodesStr, child);
		for (int j = 0; j < nodesStr.size(); j++){
			int node = nodesStr[j].ToInt();
			node2lobe[node] = i;
		}
		mOptions << dynamic_cast<const MyGenericHierarchyOverlayStr*>(child)->GetObject();
	}

	for (int i = 0; i < node2lobe.size(); i++){
		assert(node2lobe[i] >= 0);
	}

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

	int maxChange = 0;
	int varLobe = -1;
	for (int i = 0; i < mHierarchy->GetNumChildren(); i++){
		int change = lobeNumEdge0[i] - lobeNumEdge1[i];
		if (change < 0) change = -change;
		if (change > maxChange){
			maxChange = change;
			varLobe = i;
		}
	}
	mAnswerIndex = varLobe;
}

void MyGraphClusterVariationTask::AdjustKnot(MyGraphKnot* graphKnot){
	graphKnot->SetSelectionLevel(1);
}

MyString MyGraphClusterVariationTask::GetTaskName() const{
	return "Task_variation";
}