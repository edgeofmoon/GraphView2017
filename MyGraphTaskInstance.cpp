#include "MyGraphTaskInstance.h"
#include "MyRenderScene.h"
#include "MyMultiView.h"
#include "MyAntiAliasingView.h"
#include "MyGraphChoiceTask.h"
#include "MyGraphKnot.h"

#include "MyGraphTaskUI.h"
#include "MyGraphMultiNodeCommonNeighborCountTask.h"
#include "MyGraphHighestCommonNeighborTask.h"
#include "MyGraphSingleNodeDegreeCountTask.h"
#include "MyMultiGraphNodeVariationTask.h"
#include "MyGraphNodeEdgeWeightTask.h"
#include "MyGraphHubNodeTask.h"
#include "MyGraphClusterVariationTask.h"
#include "MyGraphClusterVariationComparetask.h"
#include "MyGraphActiveClusterTask.h"
#include "MyGraphClusterNodeTask.h"
#include "MyGraphInterClusterConnectionTask.h"
#include "MyGraphOppositeHubTask.h"

#include "MyDenseGraph.h"
#include "MyGraphFilter.h"

#include "MyDataLoader.h"

#include "MyGraphLayout.h"
#include "MyGraphMatrixLayout.h"
#include "MyGraphFruchtermanReingoldLayout.h"
#include "MyGraphLocalOptimizationLayout.h"
#include "MyGraphHierarchicalLayout.h"
#include "MyGraphLodLayout.h"
#include "MyGraphRadialLayout.h"

#include "MyGraphRepresentation.h"
#include "MyGraphEncodingRepresentation.h"
#include "MyGraphLodRepresentation.h"
#include "MyGraphMatrixRepresentation.h"
#include "MyGraphMatrixEncodingRepresentation.h"
#include "MyGraphHierarchicalRepresentation.h"

#include "MyGraphSelectionMask.h"

#include "MyGraphEdgeBundler.h"
#include "MyGraphClusterEdgeBundler.h"
#include "MyGraphGeometryEdgeBundler.h"

#include "MyGraphAlgorithm.h"

#include "MyUtility.h"
#include "VisConfig.h"
#include "MyGraphicsTool.h"

#include <time.h>

using namespace std;

int MyGraphTaskInstance::LastRecordedTrialIdx = -1;

MyGraphTaskInstance::MyGraphTaskInstance(int idx, int total)
{
	mIdx = idx;
	mTotalTrials = total;
	//mScene = new MyRenderScene;
	mScene = new MyScene;
	mView = new MyView;
	//mView = new MyMultiView;
	//mView = new MyAntiAliasingView;
	mView->SetScene(mScene);
	mScene->SetView(mView);
	mView->Build();
	mTaskUI = new MyGraphTaskUI;
	mTaskUI->SetIndex(mIdx);
	mTaskUI->SetTotalNumTrials(total);
	mTask = 0;
	mPos = 0;
	mLabels = 0;
	mHierarchy = 0;

	mShowMode = 0;

	mIsEmpty = true;
}


MyGraphTaskInstance::~MyGraphTaskInstance(void)
{
	SafeFreeObject(mScene);
	SafeFreeObject(mView);
	SafeFreeObject(mTask);
	SafeFreeObject(mPos);
	SafeFreeObject(mLabels);
	if(mHierarchy){
		mHierarchy->DestoryAll();
	}
	SafeFreeObject(mHierarchy);
	SafeFreeObject(mTaskUI);
}

void MyGraphTaskInstance::SetShowMode(int mode){
	mShowMode = mode;
	for(int i = 0;i<mScene->GetNumKnots();i++){
		mScene->GetKnot(i)->SetShowMode(mShowMode);
	}
}

void MyGraphTaskInstance::Show(){
	if(!mTaskUI->IsPaused()){
		mView->Show();
	}
	if(mShowMode == 0){
		mTaskUI->Show();
	}
}

void MyGraphTaskInstance::LoadFromFile(const MyString& fn){
	mConfigFileName = fn;
	ifstream infile(fn);
	if(!infile.is_open()){
		return;
	}
	MyString comment;
	MyString taskType;
	infile >> comment >> taskType;
	
	SafeFreeObject(mTask);
	infile >> comment;
	mTask = this->NewTaskByType(taskType);
	
	mTask->LoadExtraInfo(infile);
	int m;
	infile >> comment >> m;
	MyString hierarchyFn,posFn, labelFn;;
	infile >> comment >> hierarchyFn >> comment >> posFn >> comment >> labelFn;
	mHierarchy = MyDataLoader::LoadHierarchyOverlay(hierarchyFn);
	mPos =  MyDataLoader::MakeArrayVecFromFile(posFn, m);

	// flip x
	MyBoundingBox box;
	for(int i = 0;i<mPos->size();i++){
		mPos->at(i)[0] *= -1;;
		box.Engulf(mPos->at(i));
	}
	MyVec3f center = box.GetCenter();
	for(int i = 0;i<mPos->size();i++){
		mPos->at(i) -= center;
	}

	mLabels = MyDataLoader::MakeArrayStrFromFile(labelFn);
	
	int numGraph;
	infile >> comment >> numGraph;
	MyGraphKnot* tmp = 0;
	float maxValue = -999999, minValue=999999;
	MyArray<MyGraphKnot*> knots;
	for(int i = 0;i<numGraph;i++){
		MyString gfn;
		infile >> comment >> gfn >> comment >> mDensity;
		MyDenseGraphf* graph = MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(gfn,m,m,mDensity);
		mTask->AddGraph(graph);
		MyString layoutType, repType, bundlerType;
		int encoding;
		infile >> comment >> layoutType >> comment >> bundlerType
			>> comment >> encoding >> comment >> repType;
		MyGraphLayout* layout = this->NewGraphLayoutByType(layoutType);
		MyGraphRepresentation* rep = this->NewGraphRepresentationByType(repType);
		mLayoutName = layoutType;
		mEncodingName = MyVis::GetVisualElementName((MyVis::MyVisualElement)encoding);
		//MyGraphLayout* layout = new MyGraphMatrixLayout;
		//MyGraphRepresentation* rep = new MyGraphMatrixEncodingRepresentation;
		MyGraphAlgorithm algor;
		algor.SetGraph(graph);
		MyArrayf values;
		if (taskType == "task_degree"){
			values = algor.GetDegreeCentrality();
		}
		else if(taskType == "task_degree_compare"){
			values = algor.GetDegreeCentrality();
		}
		else if(taskType == "task_variation"){
			values = algor.GetDegreeCentrality();
		}
		else if (taskType == "task_neighbor"){
			values = algor.GetEigenvectorCentrality();
		}
		else if(taskType == "task_hub"){
			algor.FloydWarshallWithPathReconstruction();
			values = algor.GetBetweennessCentrality();
		}
		else if (taskType == "task_inter_lobe"){
			values = algor.GetDegreeCentrality();
		}
		else if (taskType == "task_lobe_hub"){
			algor.FloydWarshallWithPathReconstruction();
			values = algor.GetBetweennessCentrality();
		}
		else if (taskType == "task_active_lobe"){
			values = algor.GetDegreeCentrality();
		}
		else//(taskType == "task_opposite_hub"){
		{
			algor.FloydWarshallWithPathReconstruction();
			values = algor.GetBetweennessCentrality();
		}
		//values = algor.GetDegreeCentrality();
		float tMinValue = values[values.GetMinimumIndex()];
		float tMaxValue = values[values.GetMaximumIndex()];
		if(tMinValue<minValue) minValue = tMinValue;
		if(tMaxValue>maxValue) maxValue = tMaxValue;
		// modify scale
		if (taskType == "task_degree"){
			minValue = 0;
			//maxValue = 0.30;
		}
		else if(taskType == "task_degree_compare"){
			minValue = 0;
			if(DATASET == 0){
			//	maxValue = 0.30;
			}
			else{
			//	maxValue = 0.18;
			}
		}
		else if(taskType == "task_variation"){
			minValue = 0;
			if(DATASET == 0){
				maxValue = 0.30;
			}
			else{
				maxValue = 0.18;
			}
		}
		else if (taskType == "task_neighbor"){
			//minValue = -0.3;
			//maxValue = 0.3;
		}
		else if (taskType == "task_hub")
		{
			minValue = 0;
			//maxValue = 0.06;
		}

		else if (taskType == "task_inter_lobe")
		{
			minValue = 0;
			if(DATASET == 0){
				maxValue = 0.30;
			}
			else{
				maxValue = 0.18;
			}
		}
		else if (taskType == "task_lobe_hub")
		{
			minValue = 0;
			//maxValue = 0.06;
		}
		else if (taskType == "task_active_lobe")
		{
			minValue = 0;
			if(DATASET == 0){
				maxValue = 0.30;
			}
			else{
				maxValue = 0.18;
			}
		}
		else//(taskType == "task_opposite_hub")
		{
			minValue = 0;
			//maxValue = 0.06;
		}
		rep->SetHightlightNodeAffectEdge(true);
		rep->SetHightlightEdgeAffectNode(true);
		MyGraphEncodingRepresentation* enRep = dynamic_cast<MyGraphEncodingRepresentation*>(rep);
		if (enRep){
			enRep->SetNodeValuef(values);
			enRep->SetNodeEncodingMethods((MyVis::MyVisualElement)encoding);
		}
		MyGraphEdgeBundler* bundler = this->NewGraphEdgeBundlerByType(bundlerType);
		MyGraphClusterEdgeBundler* _bdl = dynamic_cast<MyGraphClusterEdgeBundler*> (bundler);
		if(_bdl){
			_bdl->SetHierarchy(mHierarchy);
		}
		layout->SetEdgeBundler(bundler);
		if(bundler){
			layout->SetEdgeBundle();
		}
		
		MyGraphSelectionMask* mask = 0;
		//if(taskType == "task_variation"
		//	|| taskType == "task_inter_lobe"
		//	|| taskType == "task_lobe_hub"
		//	|| taskType == "task_active_lobe"){
			mask = new MyGraphSelectionMask;
			mask->SetGraphRepresentation(rep);
			mask->SetHierarchy(mHierarchy);
		//}

		MyGraphKnot* graphKnot = new MyGraphKnot;
		graphKnot->SetGraph(mTask->GetGraph(i));
		graphKnot->SetGraphNodePos(mPos);
		graphKnot->SetGraphNodeLabel(mLabels);
		graphKnot->SetHierarchy(mHierarchy);
		graphKnot->SetLayout(layout);
		graphKnot->SetRepresentation(rep);
		graphKnot->SetSelectionMask(mask);
		
		knots << graphKnot;


		MyGraphKnot::Connect(tmp, graphKnot);
		tmp = graphKnot;
	}

	infile >> comment >> mDifficulty;
	infile >> comment >> mBackground;

	MyVisualElementSingleValueMapper::mBackground = mBackground;
	MyGraphRepresentation::mBackground = mBackground;
	MyGraphTaskUI::mBackground = mBackground;
	MyGraphSelectionMask::mBackground = mBackground;

	if (mBackground == 0) MyGraphicsTool::SetClearColor(MyColor4f::white());
	else MyGraphicsTool::SetClearColor(MyColor4f::gray());

	infile.close();
	/*
	if(taskType == "task_active_lobe"){
		MyGraphKnot::Disconnect(knots[0],knots[1]);
		MyGraphAlgorithm algor;
		algor.SetGraph(knots[0]->GetGraph());
		MyGenericGraphf* diffGraph = algor.MakeDifferenceGraph(knots[1]->GetGraph());
		knots[0]->SetGraph(diffGraph);
		algor.SetGraph(diffGraph);
		MyArrayf values = algor.GetDegreeCentrality();
		dynamic_cast<MyGraphEncodingRepresentation*>(knots[0]->GetRepresentation())->SetNodeValuef(values);
		delete knots[1];
		knots.resize(1);
	}
	*/
	for(int i = 0;i<knots.size();i++){
		MyGraphEncodingRepresentation* enRep 
			= dynamic_cast<MyGraphEncodingRepresentation*>(knots[i]->GetRepresentation());
		if (enRep){
			enRep->SetValueRange(minValue,maxValue);
		}
		
		if(knots.size()>=2){
			MyGraphMatrixEncodingRepresentation* matRep = dynamic_cast<MyGraphMatrixEncodingRepresentation*> (knots[1]->GetRepresentation());
			if(matRep)
				matRep->SetLabelDraw(MyGraphMatrixEncodingRepresentation::TOP);
		}

		knots[i]->Build();
		mScene->AddKnot(knots[i]);
	}

	mTask->SetHierarchy(mHierarchy);
	mTask->SetLabels(mLabels);
	mTask->ComputeAnswerAndOptions();
	mTaskUI->SetTask(mTask);
	mTaskUI->SetGraphKnot(knots[0]);

	mScene->Build();
	mView->Build();

	for(int i = 0;i<mScene->GetNumKnots();i++){
		mTask->AdjustKnot(dynamic_cast<MyGraphKnot*>(mScene->GetKnot(i)));
	}

	mIsEmpty = false;
}


MyGraphChoiceTask* MyGraphTaskInstance::NewTaskByType(const MyString& type){
	if(type == "task_degree"){
		return new MyGraphNodeEdgeWeightTask;
	}
	if(type == "task_degree_compare"){
		return new MyMultiGraphNodeVariationTask;
	}
	if(type == "task_neighbor"){
		return new MyGraphMultiNodeCommonNeighborCountTask;
	}

	if(type == "task_hub"){
		return new MyGraphHubNodeTask;
	}

	if(type == "task_variation"){
		return new MyGraphClusterVariationComparetask;
	}
	
	if(type == "task_inter_lobe"){
		return new MyGraphInterClusterConnectionTask;
	}
	if(type == "task_lobe_hub"){
		return new MyGraphClusterNodeTask;
	}

	if(type == "task_active_lobe"){
		return new MyGraphActiveClusterTask;
	}

	if(type == "task_opposite_hub"){
		return new MyGraphOppositeHubTask;
	}

	return 0;
}
MyGraphLayout* MyGraphTaskInstance::NewGraphLayoutByType(const MyString& type){
	if(type == "layout_default"){
		return new MyGraphLayout;
	}

	if(type == "layout_hierarchical"){
		return new MyGraphHierarchicalLayout;
	}

	if(type == "layout_treering"){
		return new MyGraphRadialLayout;
	}
	
	if(type == "layout_lod"){
		return new MyGraphLodLayout;
	}

	if(type == "layout_matrix"){
		return new MyGraphMatrixLayout;
	}

	return 0;
}

MyGraphRepresentation* MyGraphTaskInstance::NewGraphRepresentationByType(const MyString& type){
	if(type == "representation_default"){
		return new MyGraphEncodingRepresentation;
	}

	if(type == "representation_hierarchical"){
		return new MyGraphHierarchicalRepresentation;
	}
	
	if(type == "representation_lod"){
		return new MyGraphLodRepresentation;
	}

	if(type == "representation_matrix"){
		return new MyGraphMatrixEncodingRepresentation;
	}

	return 0;
}

MyGraphEdgeBundler* MyGraphTaskInstance::NewGraphEdgeBundlerByType(const MyString& type){
	if(type == "bundler_none"){
		return 0;
	}

	if(type == "bundler_FDEB"){
		return new MyGraphEdgeBundler;
	}
	
	if(type == "bundler_HEB"){
		return new MyGraphClusterEdgeBundler;
	}
	
	if(type == "bundler_GEB"){
		return new MyGraphGeometryEdgeBundler;
	}

	return 0;
}

int MyGraphTaskInstance::EventHandler(MyGenericEvent& eve){
	if(MyGraphTaskInstance::IsAnswerSelectionEvent(eve)){
		MyKnot* knot = mView->GetKnotAt(eve.GetMouseX(), eve.GetMouseY());
		if(knot){
			int selvl = knot->GetSelectionLevel();
			int anslvl = mTask->GetAnswerSelectionLevel();
			knot->SetSelectionLevel(anslvl);
			MyGenericNode* sel = mView->MakeSeeWhat(eve.GetMouseX(), eve.GetMouseY(), knot);
			knot->SetSelectionLevel(selvl);
			MyKnot* leftKnot = mScene->GetKnot(0);
			MyGenericNode* pNode;
			if(knot == leftKnot){
				pNode = new MyGenericHierarchyOverlayStr("left");
			}
			else{
				pNode = new MyGenericHierarchyOverlayStr("right");
			}
			MyGenericNode::Connect(pNode, sel);
			int selIdx = mTask->InterpretSelectionAnswer(pNode);
			if(selIdx >= 0){
				mUserAnswerIndex = selIdx;
				mTaskUI->SetUserSelection(selIdx);
				this->SetUserAnswerIndex(selIdx);
				this->SetUserAnswerValue(mTaskUI->GetUserAnswerValue());
				MySelectionEvent& selEve = dynamic_cast<MySelectionEvent&>(eve);
				selEve.SetAction("ANS_REPLACE", mTask->InterpretSelection(selIdx));
			}
			pNode->DestoryAll();
			SafeFreeObject(pNode);
		}
		return 1;
	}

	int UIHandleRst = mTaskUI->EventHandler(eve);

	if(UIHandleRst == 0){
		return mView->EventHandler(eve);
	}
	// cancel
	if (UIHandleRst == 5){
		if(mScene->GetNumKnots()>0){
			mScene->GetKnot(0)->ReplaceSelection(0);
			mScene->GetKnot(0)->BroadCastMessage(MyKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION);
		}
	}
	// finished
	if (UIHandleRst == 2){
		// empty task does not do anything
		if (mIsEmpty) return UIHandleRst;

		this->SetUserAnswerIndex(mTaskUI->GetUserSelection());
		this->SetUserAnswerValue(mTaskUI->GetUserAnswerValue());
	}
	return UIHandleRst;
}

/*
void MyGraphTaskInstance::LogSelection(std::ostream& out){
	// empty task does not log
	if (mIsEmpty) return;
	MyKnot* knot = mScene->GetKnot(0);
	int numSel = knot->GetNumSelections();
	clock_t c_time = std::clock();
	
	MyArrayi nodeSels;
	MyArrayi edgeSels;
	if(knot->GetSelectionLevel() == 0){
		for(int i = 0;i<numSel;i++){
			const MyGenericNode* sel = knot->GetSelection(i);
			MyGenericHierarchyOverlayStr node("node");
			MyGenericHierarchyOverlayStr edge("edge");
			if(MyGenericHierarchyOverlayStr::IsObjectEqual(&node,sel)){
				for(int in = 0;in<sel->GetNumChildren();in++){
					int nodeIdx = dynamic_cast<const MyGenericHierarchyOverlayStr*>
						(sel->GetChild(in))->GetObject().ToInt();
					nodeSels << nodeIdx;
				}

			}
			else if(MyGenericHierarchyOverlayStr::IsObjectEqual(&edge,sel)){
				for(int in = 0;in<sel->GetNumChildren();in++){
					int edgeIdx = dynamic_cast<const MyGenericHierarchyOverlayStr*>
						(sel->GetChild(in))->GetObject().ToInt();
					edgeSels << edgeIdx;
				}
			}
		}
	}
	else if(knot->GetSelectionLevel() == 1){
		MyGraphKnot* gknot = dynamic_cast<MyGraphKnot*>(knot);
		for(int i = 0;i<numSel;i++){
			const MyGenericNode* sel = knot->GetSelection(i);
			for(int ic = 0;ic < gknot->GetHierarchy()->GetNumChildren();ic++){
				if(MyGenericHierarchyOverlayStr::IsObjectEqual(gknot->GetHierarchy()->GetChild(ic),sel)){
					nodeSels << ic;
				}
			}
		}
	}

	if(mLastNodeSelection == nodeSels 
		&& mLastEdgeSelection == edgeSels
		&& mIdx == LastRecordedTrialIdx){
		//selection does not change
		// do not log
		return;
	}
	out << mIdx << " " << c_time / (double) CLOCKS_PER_SEC << " [" << nodeSels.size() << "]"
		<< " [" << edgeSels.size() << "]"<< endl;
	for(int i = 0;i<nodeSels.size();i++){
		out << nodeSels[i] << " ";
	}
	out << endl;
	for(int i = 0;i<edgeSels.size();i++){
		out << edgeSels[i] << " ";
	}
	out << endl;
	mLastNodeSelection = nodeSels;
	mLastEdgeSelection = edgeSels;
	LastRecordedTrialIdx = mIdx;
}
*/

void MyGraphTaskInstance::LogSelection(std::ostream& out, const MySelectionEvent& eve){
	if (mIsEmpty) return;
	if(eve.GetActionName().empty()) return;
	if(mLastSelectoinActionName == "LKUP"
		&& mLastSelectoinActionValue == eve.GetActionValue()){
		return;
	}
	MyString decimer(" ");
	clock_t c_time = std::clock();
	out << mParticipantIndex << decimer;
	out << mIdx << decimer;
	out << mTask->GetTaskName() << decimer;
	out << mLayoutName << decimer;
	out << mEncodingName << decimer;
	out << c_time  / (double) CLOCKS_PER_SEC << decimer;
	out << eve.GetActionName() << decimer;
	out << eve.GetActionValue() << decimer;
	out << std::endl;
	LastRecordedTrialIdx = mIdx;
	mLastSelectoinActionName = eve.GetActionName();
	mLastSelectoinActionValue = eve.GetActionValue();
}

void MyGraphTaskInstance::LogTrialStartIfHavent(std::ostream& out){
	if (mIsEmpty) return;
	if(LastRecordedTrialIdx == mIdx) return;
	MyString decimer(" ");
	clock_t c_time = std::clock();
	out << mParticipantIndex << decimer;
	out << mIdx << decimer;
	out << mTask->GetTaskName() << decimer;
	out << mLayoutName << decimer;
	out << mEncodingName << decimer;
	out << c_time  / (double) CLOCKS_PER_SEC << decimer;
	out << "TRIAL_START" << decimer;
	out << std::endl;
	LastRecordedTrialIdx = mIdx;
}

void MyGraphTaskInstance::LogPauseOrResume(std::ostream& out, bool isPause){
	clock_t c_time = std::clock();
	MyString str;
	if(isPause) str = "Pause";
	else str = "Resume";
	out << mIdx << " " << c_time  / (double) CLOCKS_PER_SEC << " [" << -1 << "]" << endl;
	out << str << " " << endl;
}

void MyGraphTaskInstance::SetParticipantIndex(int idx){
	mParticipantIndex = idx;
}
void MyGraphTaskInstance::SetTotalTrialNumber(int tn){
	mTotalTrials = tn;
}


void MyGraphTaskInstance::Log(std::ostream& out){
	// empty task does not log
	if (mIsEmpty) return;

	MyString answerString;
	MyString userAnswerString;
	double duration = mTimer.GetUsedTime();
	MyString decimer("\t");
	float correctness;
	if(mTask->GetTaskName() != "Task_Degree"){
		if(mTask->GetAnswerIndex() == mUserAnswerIndex) {
			correctness = 1;
		}
		else{
			correctness = 0;
		}
		answerString = MyString(mTask->GetAnswerIndex());
		userAnswerString = MyString(mUserAnswerIndex);
	}
	else{
		float answerValue = mTask->GetAnswerValue();
		MyVec2f range = mTaskUI->GetAnswerValueRange();
		correctness = 1-fabs(mUserAnswerValue-answerValue)/(range[1]-range[0]);
		answerString = MyString(mTask->GetAnswerValue());
		userAnswerString = MyString(mUserAnswerValue);
	}
	out << mIdx << decimer
		<< mTask->GetTaskName() << decimer
		<< mLayoutName << decimer
		<< mEncodingName << decimer
		<< mDensity << decimer
		<< mDifficulty << decimer
		<< (mBackground == 0 ? "White" : "Gray") << decimer
		<< userAnswerString << decimer
		<< answerString << decimer
		<< duration << decimer
		<< correctness << std::endl;
}

void MyGraphTaskInstance::SetUserAnswerIndex(int answerIdx){
	mUserAnswerIndex = answerIdx;
}

void MyGraphTaskInstance::SetUserAnswerValue(float answerValue){
	mUserAnswerValue = answerValue;
}

MyString MyGraphTaskInstance::GetVisualElementName() const{
	return mEncodingName;
}

MyString MyGraphTaskInstance::GetLayoutName() const{
	return mLayoutName;
}

MyString MyGraphTaskInstance::GetTaskName() const{
	return mTask->GetTaskName();
}
bool MyGraphTaskInstance::IsAnswerSelectionEvent(const MyGenericEvent& eve){
	if(eve.GetEventMouseKey() == MyGenericEvent::MOUSE_KEY_RIGHT
		&& eve.GetEventKeyState() == MyGenericEvent::KEY_DOWN){
		return true;
	}
	return false;
}