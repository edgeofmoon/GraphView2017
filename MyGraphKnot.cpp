#include "MyGraphKnot.h"
#include "MyGraphDrawer.h"
#include "MyUtility.h"
#include "MyGraphLayout.h"
#include "MyGraphMatrixLayout.h"
#include "MyGraphIterativeLayout.h"
#include "MyGraphFruchtermanReingoldLayout.h"
#include "MyGraphLocalOptimizationLayout.h"
#include "MyGraphHierarchicalLayout.h"
#include "MyGraphLodLayout.h"
#include "MyGraphRepresentation.h"
#include "MyGraphLodRepresentation.h"
#include "MyGraphMatrixRepresentation.h"
#include "MyGraphHierarchicalRepresentation.h"

MyGraphKnot::MyGraphKnot(void)
{
	mTrackBall.Reshape(1024,768);
	mDrawBoundingBox = false;
	mSelectionMask = 0;
	mSelectionFromMask = false;
	mDrawSelectionMask = false;
}

MyGraphKnot::~MyGraphKnot(void)
{
	Destory();
}


void MyGraphKnot::SetShowMode(int mode) {
	mShowMode = mode;
}

void MyGraphKnot::Build(){
	
	//mLayout->SetBoundingBox(MyBoundingBox(MyVec3f(-80.f, -80.f, -80.f), MyVec3f(80.f, 80.f, 80.f)));
	mLayout->SetBoundingBox(this->GetBoundingBox());
	mLayout->SetNodePos(mInPos);
	mLayout->SetGraph(mGraph);
	mLayout->SetFlat();

	MyGraphHierarchicalLayout*_layout = dynamic_cast<MyGraphHierarchicalLayout*>(mLayout);
	if(_layout){
		_layout->SetHierarchy(mHierarchy);
	}
	
	MyGraphMatrixLayout* _matlayout = dynamic_cast<MyGraphMatrixLayout*>(mLayout);
	if(_matlayout){
		_matlayout->OrderByHierarchy(mHierarchy);
	}

	MyGraphLodLayout* _lodlayout = dynamic_cast<MyGraphLodLayout*>(mLayout);
	if(_lodlayout){
		_lodlayout->BuildLodByHierarchy(mHierarchy);
		_lodlayout->SetLevelOfDetailIndex(2);
	}

	mLayout->SyncUpdate();
	//static_cast<MyGraphIterativeLayout*>(mLayout)->Advance();
	//static_cast<MyGraphIterativeLayout*>(mLayout)->AdvanceToFinal();
	mRepresentation->SetGraphLayout(mLayout);
	mRepresentation->SetNodeLabels(mLabels);
	mRepresentation->SetSelectionLevel(mSelectionLevel);
	//mRepresentation->SetSelection(mSelection);
	
	MyGraphHierarchicalRepresentation* _rep = dynamic_cast<MyGraphHierarchicalRepresentation*>(mRepresentation);
	if(_rep){
		_rep->SetHierarchy(mHierarchy);
	}

	if(mSelectionMask){
		mSelectionMask->Update();
	}
	mRepresentation->Update();
	mRepresentation->UpdataLabel();
	
	MyGraphDrawer::StartList(mDisplayList);
	mRepresentation->Show();
	MyGraphDrawer::EndList(mDisplayList);
	mTrackBall.Translate(this->GetBoundingBox().GetCenter());
	mTrackBall.SetOrigin(this->GetBoundingBox().GetCenter());

	//mRepresentation->UpdataLabel();
}

void MyGraphKnot::PreShow(){
	mRepresentation->ShowAllCrosshairs();
}

void MyGraphKnot::Show(){
	MyGraphDrawer::PushMatrix();
	MyGraphDrawer::LoadTrackBall(&mTrackBall);
	if(mShowMode != 0){
		this->PreShow();
	}
	else{
		if(mSelectionMask){
			mSelectionMask->Show();
		}
		//mRepresentation->ShowLabels();
		MyGraphDrawer::ShowList(mDisplayList);
		//mRepresentation->Show();
		MyGraphDrawer::WirePolygon();
		if(mDrawBoundingBox){
			MyGraphDrawer::Color(MyColor4f::white());
			mLayout->GetBoundingBox().Show();
		}
		MyGraphDrawer::FillPolygon();
	}
	MyGraphDrawer::PopMatrix();
}

void MyGraphKnot::Destory(){
	MyGraphDrawer::DeleteList(mDisplayList);
	SafeFreeObject(mLayout); 
	SafeFreeObject(mRepresentation);
	SafeFreeObject(mSelectionMask);
}

bool MyGraphKnot::SeeWhat(std::vector<MyVec2i>& see){
	int maxNumItems = mLayout->GetGraph()->GetNumNodes()+mLayout->GetGraph()->GetNumEdges();
	unsigned int* buffer = new unsigned int[2*maxNumItems];
	MyGraphDrawer::InitHitBuffer(2*maxNumItems,buffer);
	MyGraphDrawer::InitNameStack();
	MyGraphDrawer::ToSelectMode();
	mDrawSelectionMask = true;
	this->Show();
	mDrawSelectionMask = false;
	int nHits = MyGraphDrawer::ToRenderMode();
	bool seeAny = MyGraphDrawer::InterpretHit(see, buffer, nHits);
	delete[] buffer;
	return seeAny;
}

bool MyGraphKnot::SeeAny(){
	//int maxNumItems = mLayout->GetGraph()->GetNumNodes() + mLayout->GetGraph()->GetNumEdges();
	//unsigned int* buffer = new unsigned int[2 * maxNumItems];
	unsigned int buffer[2000];
	MyGraphDrawer::InitHitBuffer(2000, buffer);
	MyGraphDrawer::InitNameStack();
	MyGraphDrawer::ToSelectMode();
	//this->Show();
	MyGraphDrawer::PushMatrix();
	MyGraphDrawer::LoadTrackBall(&mTrackBall);
	MyGraphDrawer::PushName(MyGraphDrawer::BOUNDINGBOX);
	MyGraphDrawer::PushName(0);
	mLayout->GetBoundingBox().Show();
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopMatrix();
	int nHits = MyGraphDrawer::ToRenderMode();
	bool seeAny = MyGraphDrawer::HitAny(buffer, nHits);
	//delete[] buffer;
	return seeAny;
}

MyGenericNode* MyGraphKnot::MakeSeeWhat(){
	//int maxNumItems = mLayout->GetGraph()->GetNumNodes()+mLayout->GetGraph()->GetNumEdges();
	unsigned int buffer[2000];
	int r = MyGraphDrawer::GetError();
	MyGraphDrawer::InitHitBuffer(2000,buffer);
	MyGraphDrawer::InitNameStack();
	MyGraphDrawer::ToSelectMode();
	mDrawSelectionMask = true;
	this->Show();
	mDrawSelectionMask = false;
	r = MyGraphDrawer::GetError();
	int nHits = MyGraphDrawer::ToRenderMode();
	MyArrayi nodeList;
	MyArrayi edgeList;
	MyArrayi maskList;
	bool seeAny = MyGraphDrawer::InterpretHit(nodeList, edgeList, maskList, buffer, nHits);
	//delete[] buffer;
	r = MyGraphDrawer::GetError();
	if (mSelectionFromMask && maskList.size() > 0){
		return mSelectionMask->MakeSelection(maskList[0]);
	}
	
	if(nodeList.size()!=0){
		if (mSelectionFromMask){
			int nodeIdx = nodeList[0];
			MyString nodeIdxStr(nodeIdx);
			const MyGenericNode* node = MyGenericHierarchyOverlayStr::GetOneWithObject(nodeIdxStr, mHierarchy);
			const MyGenericNode* pNode = node->GetParent();
			MyGenericNode* rst = pNode->MakeCopy();
			MyGenericNode* nnode = new MyGenericHierarchyOverlayStr
				(dynamic_cast<MyGenericHierarchyOverlayStr*>(rst)->GetObject());
			rst->DestoryAscendants();
			MyGenericNode::Connect(nnode,rst);
			dynamic_cast<MyGenericHierarchyOverlayStr*>(rst)->SetObject("node");
			return nnode;
		}
		else{
			return mRepresentation->MakeSelectionFromNodes(nodeList);
		}
		/*
		MyGraphLodLayout* _lodlayout = dynamic_cast<MyGraphLodLayout*>(mLayout);
		if(_lodlayout){
			MyArray<const MyGenericNode*>* nodes = _lodlayout->MakeHierarchyOverlay();
			MyGenericNode* rst = nodes->at(nodeList[0])->MakeCopy();
			delete nodes;
			return rst;
		}
		else{
			const MyGenericNode* leave = MyGenericHierarchyOverlayi::GetOneWithObject(nodeList[0],mHierarchy);
			return leave->GetParent()->MakeCopy();
		}
		*/
	}
	if (edgeList.size() != 0 && !mSelectionFromMask){
		return mRepresentation->MakeSelectionFromEdges(edgeList);
	}
	else{
		return 0;
	}
}

void MyGraphKnot::SetGraph(const MyGenericGraphf* graph){
	mGraph = graph;
}
void MyGraphKnot::SetGraphNodePos(const MyArray3f* pos){
	mInPos = pos;
}

void MyGraphKnot::SetGraphNodeLabel(const MyArrayStr* labels){
	mLabels = labels;
}

void MyGraphKnot::SetLayout(MyGraphLayout* layout){
	mLayout = layout;
}

void MyGraphKnot::SetRepresentation(MyGraphRepresentation* representation){
	mRepresentation = representation;
}

void MyGraphKnot::SetSelectionMask(MyGraphSelectionMask* selectionMask){
	mSelectionMask = selectionMask;
}

void MyGraphKnot::SetSelectionLevel(int i){
	MyKnot::SetSelectionLevel(i);
	if(i == 0){
		mSelectionFromMask = false;
	}
	else mSelectionFromMask = true;
}

void MyGraphKnot::SetHierarchy(const MyGenericNode* hierarchy){
	mHierarchy = hierarchy;
}

MyGenericNode* MyGraphKnot::MakeGraphSelectionBySemantic(const MyGenericNode* region){
	if(!region) return 0;
	// add node
	MyGraphLodLayout* _lodlayout = dynamic_cast<MyGraphLodLayout*>(mLayout);
	MyGenericHierarchyOverlayi* nodes = new MyGenericHierarchyOverlayi(MyGenericGraphf::GRAPH_NODE);
	MyArrayi* nodeArr = new MyArrayi;
	MyGenericHierarchyOverlayi::AddLeaveObjectToArray(nodeArr, region);
	for(int i = 0;i<nodeArr->size();i++){
		MyGenericHierarchyOverlayi* nodei = new MyGenericHierarchyOverlayi(nodeArr->at(i));
		nodes->AddChild(nodei);
	}
	delete nodeArr;
	return nodes;
	
}

void MyGraphKnot::HighLightSelection(){
	MyGenericNode* selCopy = 0;
	if(mSelections.size()>0){
		selCopy = new MyGenericHierarchyOverlayStr("brain");
		for(int i = 0;i<mSelections.size();i++){
			MyGenericNode* childCopy = mSelections[i]->MakeCopy();
			childCopy->DestoryAscendants();
			MyGenericNode::Connect(selCopy,childCopy);
		}
	}
	mRepresentation->ReplaceSelection(selCopy);
	mRepresentation->Update();
	MyGraphDrawer::DeleteList(mDisplayList);
	MyGraphDrawer::StartList(mDisplayList);
	mRepresentation->Show();
	MyGraphDrawer::EndList(mDisplayList);
	//this->BroadCastMessage(MyGraphKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION);
}

void MyGraphKnot::HighlightStaticSelection(){
	MyGenericNode* selCopy = 0;
	if(mStaticSelections.size()>0){
		selCopy = new MyGenericHierarchyOverlayStr("brain");
		for(int i = 0;i<mStaticSelections.size();i++){
			MyGenericNode* childCopy = mStaticSelections[i]->MakeCopy();
			childCopy->DestoryAscendants();
			MyGenericNode::Connect(selCopy,childCopy);
		}
	}
	mRepresentation->ReplaceStaticSelection(selCopy);
	mRepresentation->Update();
	MyGraphDrawer::StartList(mDisplayList);
	mRepresentation->Show();
	MyGraphDrawer::EndList(mDisplayList);
	//this->BroadCastMessage(MyGraphKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION);
}

void MyGraphKnot::Highlight(std::vector<MyVec2i>& highlight){
	// I don't like this implementation
	std::vector<bool> highlightEdges(mLayout->GetGraph()->GetNumEdges(),false);
	std::vector<bool> highlightNodes(mLayout->GetGraph()->GetNumEdges(),false);
	for(unsigned int i = 0;i<highlight.size();i++){
		if(highlight[i][0] == MyGenericGraphf::GRAPH_EDGE){
			highlightEdges[highlight[i][1]] = true;
		}
		else if(highlight[i][0] == MyGenericGraphf::GRAPH_NODE){
			highlightNodes[highlight[i][1]] = true;
		}
	}
	MyGraphDrawer::StartList(mDisplayList);
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	for(int i = 0;i<mLayout->GetGraph()->GetNumEdges();i++){
		MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
		MyGraphDrawer::LoadName(i);
		MyGraphDrawer::Color(highlightEdges[i]?MyColor4f::yellow():MyColor4f::black());
		MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[0]),mLayout->GetNodePos(idx2[1]));
	}
	MyGraphDrawer::PopName();
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	for(int i = 0;i<mLayout->GetGraph()->GetNumNodes();i++){
		MyGraphDrawer::LoadName(i);
		MyGraphDrawer::Color(highlightNodes[i]?MyColor4f::yellow():MyColor4f::black());
		MyGraphDrawer::DrawNodeAt(mLayout->GetNodePos(i),1.f);
	}
	MyGraphDrawer::PopName();
	MyGraphDrawer::WirePolygon();
	mLayout->GetBoundingBox().Show();
	MyGraphDrawer::FillPolygon();
	MyGraphDrawer::EndList(mDisplayList);
}

MyGraphRepresentation* MyGraphKnot::GetRepresentation(){
	return mRepresentation;
}

const MyGraphRepresentation* MyGraphKnot::GetRepresentation() const{
	return mRepresentation;
}
MyGraphLayout* MyGraphKnot::GetLayout(){
	return mLayout;
}
const MyGraphLayout* MyGraphKnot::GetLayout() const{
	return mLayout;
}

MyGraphSelectionMask* MyGraphKnot::GetSelectionMask(){
	return mSelectionMask;
}

const MyGraphSelectionMask* MyGraphKnot::GetSelectionMask() const{
	return mSelectionMask;
}

//int MyGraphKnot::EventHandler(const MyGenericEvent& eve){
//	return MyGenericEventInterpreter::InterpretEvent(eve, this);
//}

const MyGenericGraphf* MyGraphKnot::GetGraph() const{
	return mGraph;
}

const MyGenericNode* MyGraphKnot::GetHierarchy() const{
	return mHierarchy;
}

const MyArray3f* MyGraphKnot::GetGraphNodePos() const{
	return mInPos;
}

const MyArrayStr* MyGraphKnot::GetGraphNodeLabel() const{
	return mLabels;
}

int MyGraphKnot::keyPressEventHandler(MyGenericEvent& eve){
	
	if(!IsAnswering()) return 0;
	MyGraphLodLayout* _lodlayout = dynamic_cast<MyGraphLodLayout*>(mLayout);
	if(_lodlayout){
		char c = eve.GetEventKeyboardKey();
		switch(c){
		case '0':
		case '1':
		case '2':
			_lodlayout->SetLevelOfDetailIndex(c-'0');
			_lodlayout->Update();
			//mRepresentation->SetSelection(mSelection);
			mRepresentation->Update();
			mRepresentation->UpdataLabel();
			MyGraphDrawer::StartList(mDisplayList);
			mRepresentation->Show();
			MyGraphDrawer::EndList(mDisplayList);
			eve.Handled();
			return 1;
			break;
		default:
			return 0;
			break;
		}
	}
	return 0;
}
int MyGraphKnot::mousePressEventHandler(MyGenericEvent& eve){
	//if(MyGenericEvent::MOUSE_KEY_LEFT == button){
	int _x = eve.GetMouseX();
	int _y = eve.GetMouseY();
		mTrackBall.StartMotion(_x,_y);
	//}
	//eve.Handled();
	return 0;
}

int MyGraphKnot::mouseReleaseEventHandler(MyGenericEvent& eve){
	//if(MyGenericEvent::MOUSE_KEY_LEFT == button){
	int _x = eve.GetMouseX();
	int _y = eve.GetMouseY();
		mTrackBall.EndMotion(_x,_y);
	//}
	if(MyGenericEvent::MOUSE_KEY_LEFT == eve.GetEventMouseKey()){
		//mRepresentation->UpdataLabel();
		//eve.SetNeedRedraw();
	}
	//eve.Handled();
	return 1;
}

int MyGraphKnot::mouseMoveEventHandler(MyGenericEvent& eve){
	MyGenericEvent::MyMouseKey _mouseKey = eve.GetEventMouseKey();
	int _x = eve.GetMouseX();
	int _y = eve.GetMouseY();
	// the drag event
	if(eve.GetEventKeyState() == MyGenericEvent::KEY_DOWN){
		// the drag event requres it being answering
		//if(!IsAnswering()) return 0;
		switch (_mouseKey){
			case MyGenericEvent::MOUSE_KEY_LEFT:
				//mTrackBall.RotateMotion(_x, _y);
				//eve.SetNeedRedraw();
				break;
			case MyGenericEvent::MOUSE_KEY_MIDDLE:
				//mTrackBall.TranslateMotion(_x, _y);
				//eve.SetNeedRedraw();
				break;
			case MyGenericEvent::MOUSE_KEY_RIGHT:
				//mTrackBall.ScaleMotion(_x, _y);
				//eve.SetNeedRedraw();
				break;
			default:
				break;
		}
	}
	// the hint event
	else{
		// the long ugly code
		for(int iCluster = 0;iCluster<mHierarchy->GetNumChildren();iCluster++){
			MyArrayStr children;
			MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&children,mHierarchy->GetChild(iCluster));
			MyBoundingBox box;
			for(int iChild = 0;iChild<children.size();iChild++){
				MyVec3f pos = (*mInPos)[children[iChild].ToInt()];
				box.Engulf(pos);
			}
			//mClusterBoundingBox << box;
		}

		int hoverBoxIdx = 0;
		if(_y!=0) hoverBoxIdx = -1;

		MyGenericNode* sel = 0;
		if(hoverBoxIdx >= 0){
			sel = mHierarchy->GetChild(hoverBoxIdx)->MakeCopy();
			sel->DestoryAscendants();
		}

		if(this->DoIncludeSelection(sel)){
			int dontneedredraw = 1;
			sel->DestoryAll();
			SafeFreeObject(sel);
		}
		else{
			this->ReplaceSelection(sel);
			this->BroadCastMessage(MyKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION);
			eve.SetNeedRedraw();
		}
		eve.Handled();
	}
	return 1;
}

int MyGraphKnot::idleEventHandler(MyGenericEvent& eve){
	return 0;
	if(static_cast<MyGraphIterativeLayout*>(mLayout)->ShouldContinue()){
		
		static_cast<MyGraphIterativeLayout*>(mLayout)->Advance();
		MyGraphDrawer::StartList(mDisplayList);
		MyGraphRepresentation rep;
		//MyGraphMatrixRepresentation rep;
		rep.SetGraphLayout(mLayout);
		rep.Show();
		//MyGraphDrawer::WirePolygon();
		//mLayout->GetBoundingBox().Show();
		//MyGraphDrawer::FillPolygon();
		MyGraphDrawer::EndList(mDisplayList);
		
		eve.Visited();
		eve.SetNeedRedraw();
		return 2;
	}
	eve.Visited();
	return 0;
}

int MyGraphKnot::windowResizeEventHandler(MyGenericEvent& eve){
	int _w = eve.GetWindowWidth();
	int _h = eve.GetWindowHeight();
	mTrackBall.Reshape(_w,_h);
	//mRepresentation->UpdataLabel();
	eve.Visited();
	return 0;
}

void MyGraphKnot::responseBroadcast(const MyKnot* source, int type){
	switch(type){
	case MyGraphKnot::BROADCAST_TYPE_NONE:
		break;
	case MyGraphKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION:
		if(this != source){
			this->replaceSelections(source);
		}
		this->HighLightSelection();
		break;
	default:
		break;
	}
}