#include "MyGraphLodRepresentation.h"
#include "MyGraphLodLayout.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphDrawer.h"
#include "MyUtility.h"
#include "My1DSampler.h"

MyGraphLodRepresentation::MyGraphLodRepresentation(void)
{
	mNodeOverlays = 0;
	mHierarchyEncodeLevel = 1;
}


MyGraphLodRepresentation::~MyGraphLodRepresentation(void)
{
}

void MyGraphLodRepresentation::Update(){
	MyGraphRepresentation::Update();
	const MyGraphLodLayout* lodLayout
		= dynamic_cast<const MyGraphLodLayout*> (mLayout);
	if(lodLayout){
		SafeFreeObject(mNodeOverlays);
		mNodeOverlays = lodLayout->MakeHierarchyOverlay();
	}
	
	mSelectedNodes.clear();
	mSelectedEdges.clear();
	this->addToSelectionList(mSelection);
	this->updateLodColors();
}


void MyGraphLodRepresentation::ShowNode(int i){
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(i);
	MyGraphDrawer::Color(mNodeColors[i]);
	MyGraphDrawer::DrawNodeAt(mLayout->GetNodePos(i));
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphLodRepresentation::ShowEdge(int i){
	MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	MyGraphDrawer::EnableAlplaBlending();
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(MyGenericGraphf::EncodeEdge(idx2));

	// edge width
	const MyGraphLodLayout* lodLayout
		= dynamic_cast<const MyGraphLodLayout*> (mLayout);
	if(lodLayout){
		int lodIdx = lodLayout->GetLevelOfDetailIndex();
		if(lodIdx<=mHierarchyEncodeLevel){
			MyGraphDrawer::SetLineWidth(fabs(mLayout->GetGraph()->GetEdgeWeight(i)));
		}
	}


	// bundle edges
	if(mLayout->GetEdgeBundle()){
		const MyArray<MyArray3f>* edgeControlPoints
			= mLayout->GetEdgeBundler()->GetControPoints();
		if(i<edgeControlPoints->size()){
			const MyArray3f& edgePoints
				= edgeControlPoints->at(i);
			MyGraphDrawer::BeginLineStrip();
			MyGraphDrawer::Color(mNodeColors[idx2[1]]*0.5f);
			for(int i = 1;i<edgePoints.size()/2;i++){
				MyGraphDrawer::Vertex(edgePoints[i]);
			}
			
			MyGraphDrawer::Color(mNodeColors[idx2[0]]*0.5f);
			for(int i = edgePoints.size()/2;i<edgePoints.size();i++){
				MyGraphDrawer::Vertex(edgePoints[i]);
			}
			MyGraphDrawer::EndPrimitive();
		}
	}
	else{
		MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetEdgeWeight(i)]);
		//MyGraphDrawer::SetLineWidth(fabs(mEdgeSizeScale[mLayout->GetGraph()->GetEdgeWeight(i)]));
		MyVec3f middlePoint = (mLayout->GetNodePos(idx2[0])+mLayout->GetNodePos(idx2[1]))/2;
		MyGraphDrawer::Color(mNodeColors[idx2[1]]*0.5f);
		MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[0]), middlePoint);
		MyGraphDrawer::Color(mNodeColors[idx2[0]]*0.5f);
		MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[1]), middlePoint);
	}
	MyGraphDrawer::SetLineWidth(1.f);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
	MyGraphDrawer::DisableAlplaBlending();
}

void MyGraphLodRepresentation::ShowHighlightedNode(int i){
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(i);
	MyGraphDrawer::Color(2*mNodeColors[i]);
	MyGraphDrawer::DrawNodeAt(mLayout->GetNodePos(i),1.5f);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphLodRepresentation::ShowHighlightedEdge(int i){
	MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(MyGenericGraphf::EncodeEdge(idx2));
	MyGraphDrawer::Color(MyColor4f(1.f,1.f,0.f,1.f));
	MyGraphDrawer::SetLineWidth(1.5*fabs(mEdgeSizeScale[mLayout->GetGraph()->GetEdgeWeight(i)]));
	MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[0]), mLayout->GetNodePos(idx2[1]));
	MyGraphDrawer::SetLineWidth(1.f);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphLodRepresentation::UpdataLabel(){
	const MyGraphLodLayout* lodLayout
		= dynamic_cast<const MyGraphLodLayout*> (mLayout);
	if(lodLayout){
		mCurrentLabels.clear();
		MyArray<const MyGenericNode*>* overlay = lodLayout->MakeHierarchyOverlay();
		for(int i = 0;i<overlay->size();i++){
			const MyGenericHierarchyOverlayStr* overlayi
				= dynamic_cast<const MyGenericHierarchyOverlayStr*>(overlay->at(i));
			if(overlayi->HasAnyChild()){
					mCurrentLabels.push_back(overlayi->GetObject());
			}
			else{
				mCurrentLabels.push_back(mLabels->at(overlayi->GetObject().ToInt()));
			}
		}
		delete overlay;
	}
}

void MyGraphLodRepresentation::ShowLabels(){
	const MyGraphLodLayout* lodLayout
		= dynamic_cast<const MyGraphLodLayout*> (mLayout);
	if(lodLayout){
		int lodIdx = lodLayout->GetLevelOfDetailIndex();
		if(lodIdx<=mHierarchyEncodeLevel){
			MyGraphDrawer::Color(MyColor4f(1.f,1.f,0.f,1.f));
			for(int i = 0; i < mLayout->GetGraph()->GetNumNodes(); ++i)
			{
				MyVec3f pos = mLayout->GetNodePos(i);
			//	MyGraphDrawer::DrawBitMapText(pos,mCurrentLabels.at(i));
			}
		}
		else{
			MyGraphDrawer::Color(MyColor4f(1.f,1.f,0.f,1.f));
			for(int i = 0; i < mLayout->GetGraph()->GetNumNodes(); ++i)
			{
				if(this->DoNodeHighlight(i)){
					MyVec3f pos = mLayout->GetNodePos(i);
					MyGraphDrawer::DrawBitMapText(pos,mCurrentLabels.at(i));
				}
			}
		}
	}
	MyGraphDrawer::Color(MyColor4f(1.f,1.f,0.f,1.f));
	for(int i = 0; i < mLayout->GetGraph()->GetNumNodes(); ++i)
	{
		if(this->DoNodeHighlight(i)){
			MyVec3f pos = mLayout->GetNodePos(i);
			MyGraphDrawer::DrawBitMapText(pos,mCurrentLabels.at(i));
		}
	}
	
}

MyGenericNode* MyGraphLodRepresentation::MakeSelectionFromNode(int nodeIdx) const{
	if(mNodeOverlays){
		MyGenericNode* rst = mNodeOverlays->at(nodeIdx)->MakeCopy();
		return rst;
	}
	else{
		return MyGraphRepresentation::MakeSelectionFromNode(nodeIdx);
	}
}
void MyGraphLodRepresentation::addToSelectionList(const MyGenericNode* root){
	if(!root) return;

	for(int i = 0;i<mNodeOverlays->size();i++){
		if(MyGenericHierarchyOverlayStr::IsObjectEqual(root, mNodeOverlays->at(i))){
			mSelectedNodes.push_back(i);
		}
	}

	for(int i = 0;i<root->GetNumChildren();i++){
		this->addToSelectionList(root->GetChild(i));
	}
}


void MyGraphLodRepresentation::updateLodColors(){
	MyArray<MyColor4f> colors;
	float alpha = 0.5f;
	colors.push_back(MyColor4f(0.33f,0.33f,0.34f,alpha));
	colors.push_back(MyColor4f(1.00f,0.00f,0.00f,alpha));
	colors.push_back(MyColor4f(0.50f,0.50f,0.00f,alpha));
	colors.push_back(MyColor4f(0.00f,1.00f,0.00f,alpha));
	colors.push_back(MyColor4f(0.00f,0.50f,0.50f,alpha));
	colors.push_back(MyColor4f(0.00f,0.00f,1.00f,alpha));
	colors.push_back(MyColor4f(0.50f,0.00f,0.50f,alpha));
	colors.push_back(MyColor4f(0.33f,0.33f,0.34f,alpha));
	My1DSampler<MyColor4f> nodeColors;
	nodeColors.SetValues(colors);

	const MyGraphLodLayout* lodLayout
		= dynamic_cast<const MyGraphLodLayout*> (mLayout);
	if(lodLayout){
		int currentLod = lodLayout->GetLevelOfDetailIndex();
		mNodeColors.clear();
		if(currentLod<mHierarchyEncodeLevel){
			MyArray<const MyGenericNode*>* colorEncodeLevelOverlay = lodLayout->GetHierarchy()->MakeArrayWithCutLeaveNoDeeperThan(mHierarchyEncodeLevel);
			for(int i = 0;i<mNodeOverlays->size();i++){
				MyArray<const MyGenericNode*>* leaves = mNodeOverlays->at(i)->MakeArrayWithCutLeaveNoDeeperThan(mHierarchyEncodeLevel);
				MyColor4f nodeColor(0.f,0.f,0.f,1.f);
				for(int ileave = 0;ileave<leaves->size();ileave++){
					int clusterIdx = colorEncodeLevelOverlay->IndexOf(leaves->at(i));
					if(clusterIdx>=0){
						if(colorEncodeLevelOverlay->size() <= 1){
						}
						else{
							nodeColor = nodeColor*0.5+nodeColors[clusterIdx/(float)(colorEncodeLevelOverlay->size()-1)]*0.5f;
						}
					}
				}
				mNodeColors.push_back(nodeColor);
				delete leaves;
			}
			delete colorEncodeLevelOverlay;
		}
		else if(currentLod == mHierarchyEncodeLevel){
			for(int i = 0;i<mNodeOverlays->size();i++){
				if(mNodeOverlays->size() <= 1){
					mNodeColors.push_back(nodeColors[0]);
				}
				else{
					mNodeColors.push_back(nodeColors[i/(float)(mNodeOverlays->size()-1)]);
				}
			}
		}
		else{
			MyArray<const MyGenericNode*>* colorEncodeLevelOverlay = lodLayout->GetHierarchy()->MakeArrayWithCutLeaveNoDeeperThan(mHierarchyEncodeLevel);
			for(int i = 0;i<mNodeOverlays->size();i++){
				const MyGenericNode* parent = mNodeOverlays->at(i)->GetParent();
				int clusterIdx = colorEncodeLevelOverlay->IndexOf(parent);
				while(clusterIdx<0){
					parent = parent->GetParent();
					clusterIdx = colorEncodeLevelOverlay->IndexOf(parent);
				}
				if(colorEncodeLevelOverlay->size() <= 1){
					mNodeColors.push_back(nodeColors[0]);
				}
				else{
					mNodeColors.push_back(nodeColors[clusterIdx/(float)(colorEncodeLevelOverlay->size()-1)]);
				}
			}
			delete colorEncodeLevelOverlay;
		}
	}
}