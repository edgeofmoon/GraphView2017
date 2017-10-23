#include "MyGraphMatrixRepresentation.h"
#include "MyLine.h"
#include "MyGraphDrawer.h"
MyGraphMatrixRepresentation::MyGraphMatrixRepresentation(void)
{
}


MyGraphMatrixRepresentation::~MyGraphMatrixRepresentation(void)
{
}

void MyGraphMatrixRepresentation::ShowNode(int i){
	//MyGraphDrawer::PushName(MyGraphDrawer::GRAPH_NODE);
	//MyGraphDrawer::LoadName(i);
	//MyGraphDrawer::DrawNodeAt(mLayout->GetNodePos(i));
	//MyGraphDrawer::PopName();
}

void MyGraphMatrixRepresentation::ShowEdge(int i){
	MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(MyGenericGraphf::EncodeEdge(idx2));
	MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetEdgeWeight(i)]);
	float x = mLayout->GetNodePos(idx2[0])[0];
	float y = mLayout->GetNodePos(idx2[1])[1];
	if(x>y){
		int t = x;
		x = y;
		y = t;
	}
	MyGraphDrawer::DrawQuadAt(MyVec3f(x,y,0), 
		MyVec3f(x+1,y,0),
		MyVec3f(x+1,y+1,0),
		MyVec3f(x,y+1,0));
	//MyGraphDrawer::DrawQuadAt(MyVec3f(y,x,0), 
	//	MyVec3f(y,x+1,0),
	//	MyVec3f(y+1,x+1,0),
	//	MyVec3f(y+1,x,0));
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphMatrixRepresentation::ShowHighlightedNode(int i){
	int n = (float)mLayout->GetGraph()->GetNumNodes();
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(i);
	MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetNodeWeight(i)]);
	MyGraphDrawer::SetLineWidth(0.f);
	MyGraphDrawer::WirePolygon();
	MyVec3f pos = mLayout->GetNodePos(i);
	float x = pos[0];
	float y = pos[1];

	int nodeBefore = mLayout->GetNodeAtPos(pos-MyVec3f(1.f,1.f,0.f));
	int nodeAfter = mLayout->GetNodeAtPos(pos+MyVec3f(1.f,1.f,0.f));
	bool highlightBefore = this->DoNodeHighlight(nodeBefore);
	bool highlightAfter = this->DoNodeHighlight(nodeAfter);

	MyLine3f left1(MyVec3f(-n/2,y,0.f), MyVec3f(-n/2,y+1,0.f));
	MyLine3f right1(MyVec3f(n/2,y,0.f), MyVec3f(n/2,y+1,0.f));
	MyLine3f top1(MyVec3f(x,n/2,0.f), MyVec3f(x+1,n/2,0.f));
	MyLine3f bottom1(MyVec3f(x,-n/2,0.f), MyVec3f(x+1,-n/2,0.f));
	MyGraphDrawer::DrawLine(left1);
	MyGraphDrawer::DrawLine(right1);
	MyGraphDrawer::DrawLine(top1);
	MyGraphDrawer::DrawLine(bottom1);

	MyLine3f left2(MyVec3f(x,-n/2,0.f), MyVec3f(x,n/2,0.f));
	MyLine3f right2(MyVec3f(x+1,-n/2,0.f), MyVec3f(x+1,n/2,0.f));
	MyLine3f top2(MyVec3f(-n/2,y+1,0.f), MyVec3f(n/2,y+1,0.f));
	MyLine3f bottom2(MyVec3f(-n/2,y,0.f), MyVec3f(n/2,y,0.f));

	if(highlightBefore && !highlightAfter){
		MyGraphDrawer::DrawLine(right2);
		MyGraphDrawer::DrawLine(top2);
	}
	else if(!highlightBefore && highlightAfter){
		MyGraphDrawer::DrawLine(left2);
		MyGraphDrawer::DrawLine(bottom2);
	}
	else if(highlightBefore && highlightAfter){
	}
	else{
		MyGraphDrawer::DrawLine(right2);
		MyGraphDrawer::DrawLine(top2);
		MyGraphDrawer::DrawLine(left2);
		MyGraphDrawer::DrawLine(bottom2);
	}
	MyGraphDrawer::FillPolygon();
	MyGraphDrawer::SetLineWidth(1.0f);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphMatrixRepresentation::ShowHighlightedEdge(int i){
	MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(MyGenericGraphf::EncodeEdge(idx2));
	MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetEdgeWeight(i)]);
	float x = mLayout->GetNodePos(idx2[0])[0];
	float y = mLayout->GetNodePos(idx2[1])[1];
	if (x>y){
		int t = x;
		x = y;
		y = t;
	}
	MyGraphDrawer::DrawQuadAt(MyVec3f(x,y,0), 
		MyVec3f(x+1,y,0),
		MyVec3f(x+1,y+1,0),
		MyVec3f(x,y+1,0));
	
	// draw boarder
	MyGraphDrawer::SetLineWidth(1.5f);
	MyGraphDrawer::WirePolygon();
	MyGraphDrawer::DrawQuadAt(MyVec3f(x,y,0), 
		MyVec3f(x+1,y,0),
		MyVec3f(x+1,y+1,0),
		MyVec3f(x,y+1,0));
	MyGraphDrawer::FillPolygon();
	MyGraphDrawer::SetLineWidth(1.0f);

	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}
void MyGraphMatrixRepresentation::update(){
	MyGraphRepresentation::Update();
}

void MyGraphMatrixRepresentation::ShowLabels(){
	//MyGraphDrawer::PushMatrix();
	//MyGraphDrawer::PushProjectionMatrix();
	//MyMatrixf identityMat = MyMatrixf::IdentityMatrix();
	//MyMatrixf viewMat = MyMatrixf::OrthographicMatrix(0,1024,0,768,1,10000);
	//MyGraphDrawer::LoadModelViewMatrix(&identityMat);
	//MyGraphDrawer::LoadProjectionMatrix(&identityMat);
	//mLabelManager.Render();
	//MyGraphDrawer::PopMatrix();
	//MyGraphDrawer::PopProjectionMatrix();
	
	MyGraphDrawer::Color(MyColor4f(1.f,1.f,0.f,1.f));
	for(int i = 0; i < mLabels->size(); ++i)
	{
		if(this->DoNodeHighlight(i)){
			MyVec3f pos = mLayout->GetNodePos(i);
			pos[0] = mLabels->size()/2;
			MyGraphDrawer::DrawBitMapText(pos,mLabels->at(i));
		}
	}
	
}