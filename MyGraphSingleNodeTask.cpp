#include "MyGraphSingleNodeTask.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphKnot.h"
#include "MyGraphRadialLayout.h"
#include "MyGraphMatrixLayout.h"
#include "VisConfig.h"

MyGraphSingleNodeTask::MyGraphSingleNodeTask(void)
{
	mNodeIdx = 0;
}


MyGraphSingleNodeTask::~MyGraphSingleNodeTask(void)
{
}


void MyGraphSingleNodeTask::SetNodeOfInterest(int idx){
	mNodeIdx = idx;
}

void MyGraphSingleNodeTask::LoadExtraInfo(std::ifstream& infile){
	infile >> mNodeIdx;
}

void MyGraphSingleNodeTask::AdjustKnot(MyGraphKnot* graphKnot){
	MyGenericNode* node = graphKnot->GetRepresentation()->MakeSelectionFromNode(mNodeIdx);
	graphKnot->AddStaticSelection(node);
	graphKnot->HighlightStaticSelection();
	
	if(graphKnot->GetGraph()->GetNumNodes() >= 99
		&& (dynamic_cast<const MyGraphRadialLayout*> (graphKnot->GetLayout()) != 0
		|| dynamic_cast<const MyGraphMatrixLayout*> (graphKnot->GetLayout()) != 0)){
		graphKnot->GetTrackBall().Translate(-graphKnot->GetRepresentation()->GetNodeTangiblePos(mNodeIdx)*SINGLEGRAPHSIZE);
	}
}