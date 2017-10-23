#include "MyGraphHierarchicalRepresentation.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphDrawer.h"

#include <cassert>
//#include "gl/glut.h"
MyGraphHierarchicalRepresentation::MyGraphHierarchicalRepresentation(void)
{
	mHierarchyRoot = 0;
}


MyGraphHierarchicalRepresentation::~MyGraphHierarchicalRepresentation(void)
{
}

void MyGraphHierarchicalRepresentation::ShowNode(int i){
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(i);
	MyGraphDrawer::Color(mClusterColors[mClusterIndices[i]/(float)(mNumClusters-1)]);
	MyGraphDrawer::DrawNodeAt(mLayout->GetNodePos(i));
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}


void MyGraphHierarchicalRepresentation::ShowEdge(int i){
	MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	MyGraphDrawer::EnableAlplaBlending();
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(i);
	if(mLayout->GetEdgeBundle()){
		const MyArray<MyArray3f>* edgeControlPoints
			= mLayout->GetEdgeBundler()->GetControPoints();
		if(i<edgeControlPoints->size()){
			const MyArray3f& edgePoints
				= edgeControlPoints->at(i);
			MyGraphDrawer::BeginLineStrip();
			/*
			MyGraphDrawer::Color(mClusterColors[mClusterIndices[idx2[1]]/(float)(mNumClusters-1)]*0.5f);
			for(int i = 1;i<edgePoints.size()/2;i++){
				MyGraphDrawer::Vertex(edgePoints[i]);
			}
			
			MyGraphDrawer::Color(mClusterColors[mClusterIndices[idx2[0]]/(float)(mNumClusters-1)]*0.5f);
			for(int i = edgePoints.size()/2;i<edgePoints.size();i++){
				MyGraphDrawer::Vertex(edgePoints[i]);
			}
			*/
			MyColor4f color1 = mClusterColors[mClusterIndices[idx2[1]]/(float)(mNumClusters-1)]*0.5f;
			MyColor4f color2 = mClusterColors[mClusterIndices[idx2[0]]/(float)(mNumClusters-1)]*0.5f;
			for(int ip = 0;ip<edgePoints.size();ip++){
				float intval = (float)ip/(edgePoints.size()-1);
				MyColor4f color = color1*(1-intval)+color2*intval;
				MyGraphDrawer::Color(color);
				MyGraphDrawer::Vertex(edgePoints[ip]);
			}
			MyGraphDrawer::EndPrimitive();
		}
	}
	else{
		MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetEdgeWeight(i)]);
		MyGraphDrawer::SetLineWidth(fabs(mEdgeSizeScale[mLayout->GetGraph()->GetEdgeWeight(i)]));
		MyVec3f middlePoint = (mLayout->GetNodePos(idx2[0])+mLayout->GetNodePos(idx2[1]))/2;
		MyGraphDrawer::Color(mClusterColors[mClusterIndices[idx2[1]]/(float)(mNumClusters-1)]*0.5f);
		MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[0]), middlePoint);
		MyGraphDrawer::Color(mClusterColors[mClusterIndices[idx2[0]]/(float)(mNumClusters-1)]*0.5f);
		MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[1]), middlePoint);
	}
	MyGraphDrawer::SetLineWidth(1.f);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
	MyGraphDrawer::DisableAlplaBlending();
}

void MyGraphHierarchicalRepresentation::ShowHighlightedNode(int i){
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(i);
	MyGraphDrawer::Color(2*mClusterColors[mClusterIndices[i]/(float)(mNumClusters-1)]);
	MyGraphDrawer::DrawNodeAt(mLayout->GetNodePos(i),1.5f);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphHierarchicalRepresentation::ShowHighlightedEdge(int i){
	//MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	//MyGraphDrawer::EnableAlplaBlending();
	//MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	//MyGraphDrawer::PushName(i);
	////MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetEdgeWeight(i)]);
	//MyGraphDrawer::SetLineWidth(1.5*fabs(mEdgeSizeScale[mLayout->GetGraph()->GetEdgeWeight(i)]));
	//MyVec3f middlePoint = (mLayout->GetNodePos(idx2[0])+mLayout->GetNodePos(idx2[1]))/2;
	//MyGraphDrawer::Color(2*mClusterColors[mClusterIndices[idx2[1]]/(float)(mNumClusters-1)]);
	//MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[0]), middlePoint);
	//MyGraphDrawer::Color(2*mClusterColors[mClusterIndices[idx2[0]]/(float)(mNumClusters-1)]);
	//MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[1]), middlePoint);
	//MyGraphDrawer::SetLineWidth(1.f);
	//MyGraphDrawer::PopName();
	//MyGraphDrawer::PopName();
	//MyGraphDrawer::DisableAlplaBlending();

	///
	MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	MyGraphDrawer::EnableAlplaBlending();
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(i);
	if(mLayout->GetEdgeBundle()){
		const MyArray<MyArray3f>* edgeControlPoints
			= mLayout->GetEdgeBundler()->GetControPoints();
		if(i<edgeControlPoints->size()){
			const MyArray3f& edgePoints
				= edgeControlPoints->at(i);
			MyGraphDrawer::SetLineWidth(2.f);
			MyGraphDrawer::BeginLineStrip();
			/*
			MyGraphDrawer::Color(2*mClusterColors[mClusterIndices[idx2[1]]/(float)(mNumClusters-1)]);
			for(int i = 1;i<edgePoints.size()/2;i++){
				MyGraphDrawer::Vertex(edgePoints[i]);
			}
			
			MyGraphDrawer::Color(2*mClusterColors[mClusterIndices[idx2[0]]/(float)(mNumClusters-1)]);
			for(int i = edgePoints.size()/2;i<edgePoints.size();i++){
				MyGraphDrawer::Vertex(edgePoints[i]);
			}
			*/
			MyColor4f color1 = 2*mClusterColors[mClusterIndices[idx2[1]]/(float)(mNumClusters-1)];
			MyColor4f color2 = 2*mClusterColors[mClusterIndices[idx2[0]]/(float)(mNumClusters-1)];
			for(int ip = 0;ip<edgePoints.size();ip++){
				float intval = (float)ip/(edgePoints.size()-1);
				MyColor4f color = color1*(1-intval)+color2*intval;
				MyGraphDrawer::Color(color);
				MyGraphDrawer::Vertex(edgePoints[ip]);
			}
			MyGraphDrawer::EndPrimitive();
		}
	}
	else{
		MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetEdgeWeight(i)]);
		MyGraphDrawer::SetLineWidth(fabs(mEdgeSizeScale[mLayout->GetGraph()->GetEdgeWeight(i)]));
		MyVec3f middlePoint = (mLayout->GetNodePos(idx2[0])+mLayout->GetNodePos(idx2[1]))/2;
		MyGraphDrawer::Color(mClusterColors[mClusterIndices[idx2[1]]/(float)(mNumClusters-1)]);
		MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[0]), middlePoint);
		MyGraphDrawer::Color(mClusterColors[mClusterIndices[idx2[0]]/(float)(mNumClusters-1)]);
		MyGraphDrawer::DrawEdgeAt(mLayout->GetNodePos(idx2[1]), middlePoint);
	}
	MyGraphDrawer::SetLineWidth(1.f);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
	MyGraphDrawer::DisableAlplaBlending();
}

void MyGraphHierarchicalRepresentation::SetHierarchy(const MyGenericNode* hierarchyRoot){
	mHierarchyRoot = hierarchyRoot;
	this->updateNodeClusterIndex();
	this->buildClusterColors();
}

void MyGraphHierarchicalRepresentation::updateNodeClusterIndex(){
	if(!mHierarchyRoot) return;
	int nClusters = mHierarchyRoot->GetNumChildren();
	int nLeaves = 0;
	for(int i = 0;i<mHierarchyRoot->GetNumChildren();i++){
		nLeaves += mHierarchyRoot->GetChild(i)->GetNumChildren();
	}
	mClusterIndices.resize(nLeaves,-1);
	for(int i = 0;i<mHierarchyRoot->GetNumChildren();i++){
		const MyGenericNode* child = mHierarchyRoot->GetChild(i);
		for(int j = 0;j<child->GetNumChildren();j++){
			const MyGenericHierarchyOverlayStr* leave = 
				dynamic_cast<const MyGenericHierarchyOverlayStr*>(child->GetChild(j));
			assert(leave);
			int nodeIdx = leave->GetObject().ToInt();
			mClusterIndices[nodeIdx] = i;
		}
	}
}

void MyGraphHierarchicalRepresentation::buildClusterColors(){
	mNumClusters = mHierarchyRoot->GetNumChildren();
	std::vector<MyColor4f> colors;
	float alpha = 0.5f;
	colors.push_back(MyColor4f(0.33f,0.33f,0.34f,alpha));
	colors.push_back(MyColor4f(1.00f,0.00f,0.00f,alpha));
	colors.push_back(MyColor4f(0.50f,0.50f,0.00f,alpha));
	colors.push_back(MyColor4f(0.00f,1.00f,0.00f,alpha));
	colors.push_back(MyColor4f(0.00f,0.50f,0.50f,alpha));
	colors.push_back(MyColor4f(0.00f,0.00f,1.00f,alpha));
	colors.push_back(MyColor4f(0.50f,0.00f,0.50f,alpha));
	colors.push_back(MyColor4f(0.33f,0.33f,0.34f,alpha));
	mClusterColors.SetValues(colors);
}