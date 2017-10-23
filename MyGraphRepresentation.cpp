#include "MyGraphRepresentation.h"

#include "MyGraphDrawer.h"
#include "Label.h"
#include "MyMatrix.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyUtility.h"


//#include "gl/glut.h"
int MyGraphRepresentation::mBackground = 0;

MyGraphRepresentation::MyGraphRepresentation(void)
{
	mLayout = 0;
	mLabels = 0;
	mSelection = 0;
	mStaticSelection = 0;
	mSelectionLevel = 0;
	mHighlightNodeAffectEdge = false;
	mHighlightEdgeAffectNode = false;
	mLabelBuilt = false;
}


MyGraphRepresentation::~MyGraphRepresentation(void)
{
}

void MyGraphRepresentation::Show(){
	// unimportant label first
	if(this->DoNodesShow()){
		for(int i = 0;i<mLayout->GetGraph()->GetNumNodes();i++){
			if(this->DoNodeShow(i) && mLabelShow[i]){
				if(this->DoNodeStaticHighlight(i)){
					//this->ShowNodeCrosshair(i);
				}
				else if(this->DoNodeHighlight(i)){
				}else{
					this->ShowLabel(i);
				}
			}
		}
	}

	if(this->DoEdgesShow()){
		for(int i = 0;i<mLayout->GetGraph()->GetNumEdges();i++){
			if(this->DoEdgeShow(i)){
				if(!this->DoEdgeHighlight(i) && !this->DoEdgeStaticHighlight(i)){
					this->ShowEdge(i);
				}
			}
		}

		for(int i = 0;i<mLayout->GetGraph()->GetNumEdges();i++){
			if(this->DoEdgeShow(i)){
				if(this->DoEdgeStaticHighlight(i)){
					this->ShowStaticHighlightedEdge(i);
				}
				else if(this->DoEdgeHighlight(i)){
					this->ShowHighlightedEdge(i);
				}
			}
		}
	}
	if(this->DoNodesShow()){
		// node highlight
		for(int i = 0;i<mLayout->GetGraph()->GetNumNodes();i++){
			if(this->DoNodeShow(i)){
				if(this->DoNodeStaticHighlight(i)){
					this->ShowNodeHighlight(i);
				}
				else if(this->DoNodeHighlight(i)){
					this->ShowNodeHighlight(i);
				}else{
				}
			}
		}
		// not highlighted nodes
		for(int i = 0;i<mLayout->GetGraph()->GetNumNodes();i++){
			if(this->DoNodeShow(i)){
				if(this->DoNodeStaticHighlight(i)){
				}
				else if(this->DoNodeHighlight(i)){
				}else{
					this->ShowNode(i);
				}
			}
		}
		// highlighted nodes
		for(int i = 0;i<mLayout->GetGraph()->GetNumNodes();i++){
			if(this->DoNodeShow(i)){
				if(this->DoNodeStaticHighlight(i)){
					//this->ShowNodeCrosshair(i);
					this->ShowStaticHighlightedNode(i);
				}
				else if(this->DoNodeHighlight(i)){
					this->ShowHighlightedNode(i);
				}else{
				}
			}
		}
		// important label last
		for(int i = 0;i<mLayout->GetGraph()->GetNumNodes();i++){
			if(this->DoNodeShow(i) && mLabelShow[i]){
				if(this->DoNodeStaticHighlight(i)){
					//this->ShowNodeCrosshair(i);
					this->ShowLabel(i);
				}
				else if(this->DoNodeHighlight(i)){
					this->ShowLabel(i);
				}else{
				}
			}
		}
	}
	//if(mLayout->GetEdgeBundler()){
	//	mLayout->GetEdgeBundler()->ShowDebug();
	//}
}

void MyGraphRepresentation::ShowNode(int i){
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(i);
	//MyGraphDrawer::Color(MyColor4f::yellow()*0.8f);
	MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetNodeWeight(i)]);
	MyGraphDrawer::SetLineWidth(3);
	MyGraphDrawer::DrawCircle(mLayout->GetNodePos(i), 0.5f, 36);
	MyGraphDrawer::SetLineWidth(1);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphRepresentation::ShowEdge(int i){
	MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	MyGraphDrawer::EnableAlplaBlending();
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(MyGenericGraphf::EncodeEdge(idx2));
	//MyColor4f edgeColor = MyColor4f::white();
	MyColor4f edgeColor = GetEdgeDefaultColor();
	edgeColor.a = 0.3f;
	MyGraphDrawer::Color(edgeColor);
	//MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetEdgeWeight(i)]);
	MyGraphDrawer::SetLineWidth(2);
	if(mLayout->GetEdgeBundle()){
		const MyArray<MyArray3f>* edgeControlPoints
			= mLayout->GetEdgeBundler()->GetControPoints();
		/*
		if(i<edgeControlPoints->size()){
			const MyArray3f& edgePoints
				= edgeControlPoints->at(i);
			MyGraphDrawer::BeginLineStrip();
			for(int i = 1;i<edgePoints.size();i++){
				MyGraphDrawer::Vertex(edgePoints[i]);
			}
			MyGraphDrawer::EndPrimitive();
		}
		*/
		MyGraphDrawer::DrawCurve(edgeControlPoints->at(i),30);
	}
	else{
		MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[0]), mLayout->GetNodePos(idx2[1]));
	}
	MyGraphDrawer::SetLineWidth(1.f);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
	MyGraphDrawer::DisableAlplaBlending();
}

void MyGraphRepresentation::ShowHighlightedNode(int i){
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(i);
	MyGraphDrawer::Color(MyColor4f(1.f,1.f,0.f,1.f));
	MyGraphDrawer::DrawNodeAt(mLayout->GetNodePos(i),0.75f);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphRepresentation::ShowHighlightedEdge(int i){
	MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	MyGraphDrawer::EnableAlplaBlending();
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(MyGenericGraphf::EncodeEdge(idx2));
	//MyGraphDrawer::Color(MyColor4f(1.f,1.f,0.f,1.f));
	/*
	MyGraphDrawer::Color(MyColor4f::yellow());
	MyGraphDrawer::SetLineWidth(3);
	if(mLayout->GetEdgeBundle()){
		const MyArray<MyArray3f>* edgeControlPoints
			= mLayout->GetEdgeBundler()->GetControPoints();
		if(i<edgeControlPoints->size()){
			const MyArray3f& edgePoints
				= edgeControlPoints->at(i);
			MyGraphDrawer::BeginLineStrip();
			for(int i = 1;i<edgePoints.size();i++){
				MyGraphDrawer::Vertex(edgePoints[i]);
			}
			MyGraphDrawer::EndPrimitive();
		}
	}
	else{
		MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[0]), mLayout->GetNodePos(idx2[1]));
	}
	MyGraphDrawer::SetLineWidth(1.f);
	*/
	//MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetEdgeWeight(i)]);
	MyGraphDrawer::Color(this->GetEdgeColor(i));
	MyGraphDrawer::SetLineWidth(this->GetEdgeWidthf(i));
	if(mLayout->GetEdgeBundle()){
		const MyArray<MyArray3f>* edgeControlPoints
			= mLayout->GetEdgeBundler()->GetControPoints();
		/*
		if(i<edgeControlPoints->size()){
			const MyArray3f& edgePoints
				= edgeControlPoints->at(i);
			MyGraphDrawer::BeginLineStrip();
			for(int i = 1;i<edgePoints.size();i++){
				MyGraphDrawer::Vertex(edgePoints[i]);
			}
			MyGraphDrawer::EndPrimitive();
		}
		*/
		MyGraphDrawer::DrawCurve(edgeControlPoints->at(i),30);

	}
	else{
		MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[0]), mLayout->GetNodePos(idx2[1]));
	}
	MyGraphDrawer::SetLineWidth(1.f);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
	MyGraphDrawer::DisableAlplaBlending();
}

void MyGraphRepresentation::ShowNodeHighlight(int i){
}

void MyGraphRepresentation::ShowStaticHighlightedNode(int i){
	this->ShowHighlightedNode(i);
}

void MyGraphRepresentation::ShowStaticHighlightedEdge(int i){
	this->ShowHighlightedEdge(i);
}

void MyGraphRepresentation::ShowNodeCrosshair(int i){
	MyBoundingBox box = mLayout->GetBoundingBox();
	float chsize = 0.051;
	MyGraphDrawer::SetLineWidth(3);
	for(int imarker = 0;imarker < this->GetNumOfTangibleMarker(i);imarker++){
		MyVec3f pos = this->GetNodeTangiblePos(i, imarker);
		MyGraphDrawer::Color(this->GetNodeLabelColor(i));
		/*
		MyGraphDrawer::DrawLineAt(
			MyVec3f(box.GetLeft()-box.GetWidth()*chsize,pos[1],pos[2]),
			MyVec3f(box.GetLeft(),pos[1],pos[2]));
		MyGraphDrawer::DrawLineAt(
			MyVec3f(box.GetRight()+box.GetWidth()*chsize*2,pos[1],pos[2]),
			MyVec3f(box.GetRight()+box.GetWidth()*chsize,pos[1],pos[2]));
		MyGraphDrawer::DrawLineAt(
			MyVec3f(pos[0],box.GetBottom()-box.GetHeight()*chsize,pos[2]),
			MyVec3f(pos[0],box.GetBottom(),pos[2]));
		MyGraphDrawer::DrawLineAt(
			MyVec3f(pos[0],box.GetTop(),pos[2]),
			MyVec3f(pos[0],box.GetTop()+box.GetHeight()*chsize,pos[2]));

		MyGraphDrawer::Color(0.6*MyColor4f(1,1,1,1));
		MyGraphDrawer::SetLineWidth(1);
		MyGraphDrawer::DrawLineAt(
			MyVec3f(box.GetRight()+box.GetWidth()*chsize,pos[1],pos[2]),
			MyVec3f(box.GetLeft(),pos[1],pos[2]));
		MyGraphDrawer::DrawLineAt(
			MyVec3f(pos[0],box.GetBottom(),pos[2]),
			MyVec3f(pos[0],box.GetTop(),pos[2]));
		*/
		MyGraphDrawer::DrawLineAt(
			MyVec3f(pos[0]+box.GetWidth()*chsize,pos[1],pos[2],0),
			MyVec3f(pos[0]-box.GetWidth()*chsize,pos[1],pos[2]));
		MyGraphDrawer::DrawLineAt(
			MyVec3f(pos[0],pos[1]+box.GetHeight()*chsize,pos[2]),
			MyVec3f(pos[0],pos[1]-box.GetHeight()*chsize,pos[2]));
	}
	MyGraphDrawer::SetLineWidth(1);
}

void MyGraphRepresentation::ShowAllCrosshairs(){
	if(this->DoNodesShow()){
		for(int i = 0;i<mLayout->GetGraph()->GetNumNodes();i++){
			if(this->DoNodeShow(i)){
				if(this->DoNodeStaticHighlight(i)){
					this->ShowNodeCrosshair(i);
				}
			}
		}
	}
}

void MyGraphRepresentation::ShowBoundingBox(){
	MyGraphDrawer::PushName(MyGraphDrawer::BOUNDINGBOX);
	MyGraphDrawer::LoadName(0);
	MyGraphDrawer::WirePolygon();
	mLayout->GetBoundingBox().Show();
	MyGraphDrawer::FillPolygon();
	MyGraphDrawer::PopName();
}

void MyGraphRepresentation::SetGraphLayout(const MyGraphLayout* layout){
	mLayout = layout;
}
const MyGraphLayout* MyGraphRepresentation::GetGraphLayout() const{
	return mLayout;
}

void MyGraphRepresentation::ReplaceSelection(MyGenericNode* selection){
	if(mSelection){
		mSelection->DestoryAll();
		SafeFreeObject(mSelection);
	}
	mSelection = selection;
}
const MyGenericNode* MyGraphRepresentation::GetSelection() const{
	return mSelection;
}

void MyGraphRepresentation::ReplaceStaticSelection(MyGenericNode* selection){
	if(mSelection){
		mStaticSelection->DestoryAll();
		SafeFreeObject(mStaticSelection);
	}
	mStaticSelection = selection;
}

const MyGenericNode* MyGraphRepresentation::GetStaticSelection() const{
	return mStaticSelection;
}

bool MyGraphRepresentation::DoNodesShow() const{
	return true;
}
bool MyGraphRepresentation::DoEdgesShow() const{
	return true;
}
bool MyGraphRepresentation::DoNodeShow(int i) const{
	return DoNodesShow();
}
bool MyGraphRepresentation::DoEdgeShow(int i) const{
	return DoEdgesShow();
}

bool MyGraphRepresentation::DoNodeHighlight(int i) const{
	if( mSelectedNodes.HasOne(i) ){
		return true;
	};
	if(mHighlightEdgeAffectNode){
		for(int ie = 0;ie<mSelectedEdges.size();ie++){
			if(mSelectedEdges[ie][0] == i || mSelectedEdges[ie][1] == i){
				return true;
			}
		}
	}
	return false;
}


bool MyGraphRepresentation::DoEdgeHighlight(int i) const{
	if(mSelectedEdges.HasOne(mLayout->GetGraph()->GetEdge(i))){
		return true;
	}
	if(mHighlightNodeAffectEdge){
		MyVec2i edge = mLayout->GetGraph()->GetEdge(i);

		return mSelectedNodes.HasOne(edge[0]) || mSelectedNodes.HasOne(edge[1]);
	}
	return false;
}

bool MyGraphRepresentation::DoNodeStaticHighlight(int i) const{
	return mStaticSelectedNodes.HasOne(i);
}
bool MyGraphRepresentation::DoEdgeStaticHighlight(int i) const{
	return mStaticSelectedEdges.HasOne(mLayout->GetGraph()->GetEdge(i));
}

bool MyGraphRepresentation::DoBoundingBoxShow() const{
	return true;
}

void MyGraphRepresentation::Update(){
	
	const MyGenericGraphf* graph = mLayout->GetGraph();
	float minWeight = graph->GetEdgeWeight(0);
	float maxWeight = minWeight;
	for(int i = 0;i<graph->GetNumEdges();i++){
		float weight = fabs(graph->GetEdgeWeight(i));
		if(weight>maxWeight){
			maxWeight = weight;
		}
		else if(weight<minWeight){
			minWeight = weight;
		}
	}

	mColorScheme.SetDomain(minWeight,maxWeight);
	//mColorScheme.SetRange(MyColor4f(0.f,0.f,0.f,0.5f),MyColor4f(1.f,1.f,1.f,0.5f));
	mColorScheme.SetRange(MyColor4f(0.f,0.f,1.f,0.5f),MyColor4f(1.f,1.f,1.f,0.5f));
	
	mEdgeSizeScale.SetDomain(minWeight,maxWeight);
	mEdgeSizeScale.SetRange(1.f,5.f);
	
	mSelectedNodes.clear();
	mStaticSelectedNodes.clear();
	mSelectedEdges.clear();
	this->addToSelectionList(mSelection);
	this->addToStaticSelectionList(mStaticSelection);
}

void MyGraphRepresentation::SetNodeLabels(const MyArrayStr* nodeLabels){
	mLabels = nodeLabels;
}

void MyGraphRepresentation::ShowLabel(int idx){
}

void MyGraphRepresentation::ShowLabels(){
	//MyGraphDrawer::PushMatrix();
	//MyGraphDrawer::PushProjectionMatrix();
	//MyMatrixf identityMat = MyMatrixf::IdentityMatrix();
	//MyMatrixf viewMat = MyMatrixf::OrthographicMatrix(0,1024,0,768,1,10000);
	//MyGraphDrawer::LoadModelViewMatrix(&identityMat);
	//MyGraphDrawer::LoadProjectionMatrix(&identityMat);
	//mLabelManager.Render();
	//MyGraphDrawer::PopMatrix();
	//MyGraphDrawer::PopProjectionMatrix();
	
	for(int i = 0; i < mLayout->GetGraph()->GetNumNodes(); ++i){
		this->ShowLabel(i);
	}
}


void MyGraphRepresentation::UpdataLabel(){
	//MyVec4i viewport = MyGraphDrawer::GetViewport();
	//mLabelManager = LabelManager(viewport[2], viewport[3]);
	//std::vector<MyVec3f> toAddPos;
	//MyMatrixd projMat = MyGraphDrawer::GetProjectionMatrix().Transpose();
	//MyMatrixd modelViewMat = MyGraphDrawer::GetModelViewMatrix().Transpose();
	//std::vector<std::string> toAddLabels;
	//MyBoundingBox box(MyVec3f(0,0,-1),MyVec3f(viewport[2],viewport[3],1));
	//for(int i = 0; i < mLabels.size();i++){
	//	MyVec3f _pos = mLayout->GetNodePos(i);
	//	MyVec3f screenPos = MyGraphDrawer::GetProjection(_pos,modelViewMat,projMat,viewport);
	//	if(box.IsIn(screenPos)){
	//		toAddLabels.push_back(mLabels[i]);
	//		toAddPos.push_back(screenPos);
	//	}
	//}
	//
	//mLabelManager.Initialize(toAddLabels,toAddPos);
	//mLabelManager.Labeling();
	mLabelPos.clear();
	for(int i = 0;i<mLayout->GetGraph()->GetNumNodes();i++){
		mLabelPos << mLayout->GetNodePos(i);
	}
	mLabelShow = MyArrayb(mLayout->GetGraph()->GetNumNodes(), true);
	if(!mLabelBuilt){
		mLabelObjects.SetStrings(*mLabels);
		mLabelObjects.Build();
		mLabelBuilt = true;
	}
}

MyGenericNode* MyGraphRepresentation::MakeSelectionFromNode(int nodeIdx) const{
	MyGenericHierarchyOverlayStr* node = new MyGenericHierarchyOverlayStr("node");
	MyGenericHierarchyOverlayStr* node_c = new MyGenericHierarchyOverlayStr(MyString(nodeIdx));
	node->AddChild(node_c);
	return node;
}

MyGenericNode* MyGraphRepresentation::MakeSelectionFromEdge(int edgeIdx) const{
	MyGenericHierarchyOverlayStr* edge = new MyGenericHierarchyOverlayStr("edge");
	MyGenericHierarchyOverlayStr* edge_c = new MyGenericHierarchyOverlayStr(MyString(edgeIdx));
	edge->AddChild(edge_c);
	return edge;
}

MyGenericNode* MyGraphRepresentation::MakeSelectionFromNodes(MyArrayi nodeIdx) const{
	if(nodeIdx.size()<=0) return 0;
	MyGenericHierarchyOverlayStr* node = new MyGenericHierarchyOverlayStr("node");
	for(int i = 0;i<nodeIdx.size();i++){
		MyGenericHierarchyOverlayStr* node_c = new MyGenericHierarchyOverlayStr(MyString(nodeIdx[i]));
		node->AddChild(node_c);
	}
	return node;
}

MyGenericNode* MyGraphRepresentation::MakeSelectionFromEdges(MyArrayi edgeIdx) const{
	if(edgeIdx.size()<=0) return 0;
	MyGenericHierarchyOverlayStr* edge = new MyGenericHierarchyOverlayStr("edge");
	for(int i = 0;i<edgeIdx.size();i++){
		MyGenericHierarchyOverlayStr* edge_c = new MyGenericHierarchyOverlayStr(MyString(edgeIdx[i]));
		edge->AddChild(edge_c);
	}
	return edge;
}

void MyGraphRepresentation::SetHightlightNodeAffectEdge(bool b){
	mHighlightNodeAffectEdge = b;
}

void MyGraphRepresentation::SetHightlightEdgeAffectNode(bool b){
	mHighlightEdgeAffectNode = b;
}


const MyScale<float, MyColor4f>& MyGraphRepresentation::GetColorScale() const{
	return mColorScheme;
}

MyColor4f MyGraphRepresentation::GetNodeLabelColor(int i) const{
	if (mBackground == 0){
		// white bg
		if (this->DoNodeStaticHighlight(i)){
			if (this->DoNodeHighlight(i))return MyColor4f::orange();
			else return MyColor4f::red();
		}
		else if (this->DoNodeHighlight(i)) return MyColor4f::blue();
		else return MyColor4f::black();
	}
	else{
		// gray bg
		if (this->DoNodeStaticHighlight(i)){
			if (this->DoNodeHighlight(i))return MyColor4f::cyan();
			else return MyColor4f::red();
		}
		else if (this->DoNodeHighlight(i)) return MyColor4f::yellow();
		else return MyColor4f::white();
	}
}

MyColor4f MyGraphRepresentation::GetEdgeColor(int i) const{
	if(mSelectedEdges.HasOne(mLayout->GetGraph()->GetEdge(i))){
		return MyColor4f(1,0,0,1);
	}
	else if(mHighlightNodeAffectEdge){
		MyVec2i edge = mLayout->GetGraph()->GetEdge(i);
		if(mSelectedNodes.HasOne(edge[0]) || mSelectedNodes.HasOne(edge[1])){
			return MyColor4f(0.8,0.3,0.3,1);
		}
	}
	else {
		return this->GetEdgeDefaultColor();
	}
}

MyColor4f MyGraphRepresentation::GetEdgeDefaultColor() const{
	if (mBackground == 0) return MyColor4f(0, 0, 0, 0.5);
	else return MyColor4f(1, 1, 1, 0.5);
	
}

int MyGraphRepresentation::GetEdgeSelectionStatus(int i) const{
	if(mSelectedEdges.HasOne(mLayout->GetGraph()->GetEdge(i))){
		return 2;
	}
	else if(mHighlightNodeAffectEdge){
		MyVec2i edge = mLayout->GetGraph()->GetEdge(i);
		if(mSelectedNodes.HasOne(edge[0]) || mSelectedNodes.HasOne(edge[1])){
			return 1;
		}
	}
	else {
		return 0;
	}
}

float MyGraphRepresentation::GetEdgeWidthf(int i) const{
	return this->GetEdgeSelectionStatus(i)+1.5;
}

void MyGraphRepresentation::addToSelectionList(const MyGenericNode* root){
	if(!root) return;
	/*
	const MyGenericHierarchyOverlayi* nameNode
		= dynamic_cast<const MyGenericHierarchyOverlayi*>(root);
	if(nameNode){
		if (nameNode->GetObject() == MyGenericGraphf::GRAPH_NODE){
			MyGenericHierarchyOverlayi::AddLeaveObjectToArray(&mSelectedNodes, root);
		}
		else if(nameNode->GetObject() == MyGenericGraphf::GRAPH_EDGE){
			MyGenericHierarchyOverlay2i::AddLeaveObjectToArray(&mSelectedEdges, root);
		}
	}
	else{
		if(root->HasAnyChild()){
			for (int i = 0; i < root->GetNumChildren(); i++){
				this->addToSelectionList(root->GetChild(i));
			}
		}
	}
	*/
	MyGenericHierarchyOverlayStr nodeIndicator("node");
	MyGenericHierarchyOverlayStr edgeIndicator("edge");
	if(MyGenericHierarchyOverlayStr::IsObjectEqual(root,&nodeIndicator)){
		MyArrayStr strArr;
		if(root->HasAnyChild()){
			MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&strArr, root);
			for(int i = 0;i<strArr.size();i++){
				mSelectedNodes.push_back(strArr[i].ToInt());
			}
		}
	}
	else if(MyGenericHierarchyOverlayStr::IsObjectEqual(root,&edgeIndicator)){
		MyArrayStr strArr;
		if(root->HasAnyChild()){
			MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&strArr, root);
			for(int i = 0;i<strArr.size();i++){
				int edgeName = strArr[i].ToInt();
				MyVec2i edge = MyGenericGraphf::DecodeEdge(edgeName);
				if(mLayout->GetGraph()->GetEdgeIndex(edge)>=0){
					mSelectedEdges.push_back(edge);
				}
			}
		}
	}
	else{
		for(int i = 0;i<root->GetNumChildren();i++){
			this->addToSelectionList(root->GetChild(i));
		}
	}
}

void MyGraphRepresentation::addToStaticSelectionList(const MyGenericNode* root){
	if(!root) return;
	/*
	const MyGenericHierarchyOverlayi* nameNode
		= dynamic_cast<const MyGenericHierarchyOverlayi*>(root);
	if(nameNode){
		if (nameNode->GetObject() == MyGenericGraphf::GRAPH_NODE){
			MyGenericHierarchyOverlayi::AddLeaveObjectToArray(&mSelectedNodes, root);
		}
		else if(nameNode->GetObject() == MyGenericGraphf::GRAPH_EDGE){
			MyGenericHierarchyOverlay2i::AddLeaveObjectToArray(&mSelectedEdges, root);
		}
	}
	else{
		if(root->HasAnyChild()){
			for (int i = 0; i < root->GetNumChildren(); i++){
				this->addToSelectionList(root->GetChild(i));
			}
		}
	}
	*/
	MyGenericHierarchyOverlayStr nodeIndicator("node");
	MyGenericHierarchyOverlayStr edgeIndicator("edge");
	if(MyGenericHierarchyOverlayStr::IsObjectEqual(root,&nodeIndicator)){
		MyArrayStr strArr;
		if(root->HasAnyChild()){
			MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&strArr, root);
			for(int i = 0;i<strArr.size();i++){
				mStaticSelectedNodes.push_back(strArr[i].ToInt());
			}
		}
	}
	else if(MyGenericHierarchyOverlayStr::IsObjectEqual(root,&edgeIndicator)){
		MyArrayStr strArr;
		if(root->HasAnyChild()){
			MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&strArr, root);
			for(int i = 0;i<strArr.size();i++){
				mStaticSelectedEdges.push_back(mLayout->GetGraph()->GetEdge(strArr[i].ToInt()));
			}
		}
	}
	else{
		for(int i = 0;i<root->GetNumChildren();i++){
			this->addToStaticSelectionList(root->GetChild(i));
		}
	}
}

float MyGraphRepresentation::GetNodeSize(int i) const{
	return 1.0f;
}

float MyGraphRepresentation::GetHighlightNodeSize(int i) const{
	return 1.5f;
}

float MyGraphRepresentation::GetEdgeWidth(int i) const{
	return fabs(mEdgeSizeScale[mLayout->GetGraph()->GetEdgeWeight(i)]);
}

float MyGraphRepresentation::GetHighlightEdgeWidth(int i) const{
	return 1.5*this->GetEdgeWidth(i);
}

MyVec3f MyGraphRepresentation::GetNodeTangiblePos(int i, int mIdx) const{
	return mLayout->GetNodePos(i);
}

int MyGraphRepresentation::GetNumOfTangibleMarker(int i) const{
	return 1;
}

MyBoundingBox MyGraphRepresentation::GetNodeTangibleBox(int i, int mIdx) const{
	MyVec3f pos = this->GetNodeTangiblePos(i, mIdx);
	return MyBoundingBox(pos, pos);
}

void MyGraphRepresentation::DrawSelectableLabel(int nodeIdx, MyVec3f offset, float scale, MyQuarternion rotation){
	/*
	MyBoundingBox box = MyGraphDrawer::GetBitMapTextBox(label, offset);
	box.SquashDimension();
	MyGraphDrawer::EnableAlplaBlending();
	MyGraphDrawer::Color(MyColor4f(0, 0, 0, 0));
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(nodeIdx);
	//box.Show();	
	
	int width = MyGraphDrawer::GetBitMapTextWidth(label);
	float widthf = width*0.17;
	MyGraphicsTool::BeginQuads();
	MyGraphicsTool::Vertex(offset);
	MyGraphicsTool::Vertex(offset+MyVec3f(widthf,0,0));
	MyGraphicsTool::Vertex(offset+MyVec3f(widthf,2,0));
	MyGraphicsTool::Vertex(offset+MyVec3f(0,2,0));
	MyGraphicsTool::EndPrimitive();
	
	//MyGraphDrawer::DrawSphereAt(offset+MyVec3f(10,0,0),1);
	MyGraphDrawer::Color(this->GetNodeLabelColor(nodeIdx));
	MyGraphDrawer::SetLineWidth(1);
	MyGraphDrawer::DrawBitMapText(offset, label);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
	MyGraphDrawer::DisableAlplaBlending();
	*/
	
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(nodeIdx);
	MyGraphDrawer::PushMatrix();
	MyGraphDrawer::Translate(offset);
	MyGraphDrawer::MultiplyMatrix(&rotation.GetMatrix().Transpose());
	MyGraphDrawer::Scale(scale);
	MyGraphDrawer::Translate(MyVec3f(0,-mLabelObjects.GetTextObject(nodeIdx).GetHeight()/2,0));
	mLabelObjects.GetTextObject(nodeIdx).Show();
	MyGraphDrawer::PopMatrix();
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphRepresentation::SetSelectionLevel(int lvl){
	mSelectionLevel = lvl;
}