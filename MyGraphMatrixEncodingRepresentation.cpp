#include "MyGraphMatrixEncodingRepresentation.h"
#include "MyGraphDrawer.h"
#include "MyUtility.h"
#include "VisConfig.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphMatrixLayout.h"

#define MARKERBOARDER 1

MyGraphMatrixEncodingRepresentation::MyGraphMatrixEncodingRepresentation(void)
{
	mCellWidth = MATRIXCELLWIDTH;
	mCellHeight = MATRIXCELLHEIGHT;
	mCellBoarder_X = 0.1;
	mCellBoarder_Y = 0.1;
	mLabelDrawByte = TOP | RIGHT;
	mNodeHighlightByte = ROW | COLUMN;
	mNodeAffectEdgeHighlightByte = ROW | COLUMN;
}


MyGraphMatrixEncodingRepresentation::~MyGraphMatrixEncodingRepresentation(void)
{
}

void MyGraphMatrixEncodingRepresentation::Show(){
	// draw subtle grid lines
	MyGraphDrawer::EnableAlplaBlending();
	float n = (float)mLayout->GetGraph()->GetNumNodes();
	MyGraphDrawer::Color(MyColor4f(0.6,0.6,0.6,0.5));
	for(int i = 0;i<=n;i++){
		MyVec3f left(-n/2*mCellWidth, (i-n/2)*mCellHeight, 0);
		MyVec3f right(n/2*mCellWidth, (i-n/2)*mCellHeight, 0);
		MyVec3f bottom((i-n/2)*mCellWidth, -n/2*mCellHeight, 0);
		MyVec3f top((i-n/2)*mCellWidth, n/2*mCellHeight, 0);
		MyGraphDrawer::DrawLineAt(left, right);
		MyGraphDrawer::DrawLineAt(bottom, top);
	}
	MyGraphDrawer::DisableAlplaBlending();
	/*
	MyArray2i bounds;
	const MyGraphMatrixLayout* layout = dynamic_cast<const MyGraphMatrixLayout*>(mLayout);
	if(layout){
		const MyGenericNode* hierarchy = layout->GetHierarchy();
		if(hierarchy){
			int startIdx = 0;
			int endIdx = 0;
			for(int i = 0;i<hierarchy->GetNumChildren();i++){
				int n_des = hierarchy->GetChild(i)->GetNumChildren();
				endIdx = startIdx + n_des-1;
				bounds<<MyVec2i(startIdx, endIdx);
				startIdx = endIdx+1;
			}
		}
	}
	MyGraphDrawer::Color(MyColor4f(0.7,0.7,0.7,1));
	for(int i = 0;i<bounds.size();i++){
		this->DrawRowBox(bounds[i][0],bounds[i][1]);
		this->DrawColumnBox(bounds[i][0],bounds[i][1]);
	}
	*/
	MyGraphRepresentation::Show();
	/*
	if(mVisualElement == MyVis::LENGTH){
		float radius = mLayout->GetNodePos(0).norm();
		float incr = 1;
		float steps = 5;
		int n = (float)mLayout->GetGraph()->GetNumNodes();
		for(int i = 0;i<steps;i++){
			MyPrimitiveDrawer::DrawLineAt(
				MyVec3f(-n / 2 * mCellWidth, (i*incr + n / 2 + 1)*mCellHeight, 0),
				MyVec3f(n / 2 * mCellWidth, (i*incr + n / 2 + 1)*mCellHeight, 0));
		}
	}
	*/
}

void MyGraphMatrixEncodingRepresentation::ShowNode(int i){
	float n = (float)mLayout->GetGraph()->GetNumNodes();
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	//MyGraphDrawer::LoadName(i);
	MyGraphDrawer::PushName(i);
	MyGraphDrawer::Color(MyColor4f::white());
	/*
	MyGraphDrawer::Translate(MyVec3f(mLayout->GetNodePos(i)[0]+0.5f,n/2+1,0));
	//MyGraphDrawer::Scale(0.2f);
	int xp = mLayout->GetNodePos(i)[0]+n/2;
	if (xp % 2 == 1){
		MyGraphDrawer::Translate(MyVec3f(0,2,0));
	}
	*/
	int xp = mLayout->GetNodePos(i)[0] + n / 2;
	
	// draw marker and label on the top
	MyGraphDrawer::PushMatrix();
	MyGraphDrawer::Translate(MyVec3f((mLayout->GetNodePos(i)[1] + 0.5f) * mCellWidth,
		(n/2 + 1)*mCellHeight, 0));
	//MyGraphDrawer::Scale(0.2f);
	if (xp % 2 == 1 && mVisualElement != MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(0, MARKERSCALE, 0));
	}
	if (mVisualElement == MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(0, -MARKERSCALE/2.f + MARKERBOARDER, 0));
	}
	MyGraphDrawer::Scale(MARKERSCALE);
	mMapper.SetRandomSeed(i);
	mMapper.DrawValueMarker(mNodeValues[i]);
	MyGraphDrawer::PopMatrix();
	
	// draw marker and label on the right side
	MyGraphDrawer::PushMatrix();
	MyGraphDrawer::Translate(MyVec3f((n / 2 + 1) * mCellWidth,
		(mLayout->GetNodePos(i)[1] + 0.5f)*mCellHeight, 0));
	//MyGraphDrawer::Scale(0.2f);
	if (xp % 2 == 1 && mVisualElement != MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(MARKERSCALE, 0, 0));
	}
	if (mVisualElement == MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(-MARKERSCALE/2.f + MARKERBOARDER, 0, 0));
	}
	MyGraphDrawer::Scale(MARKERSCALE);
	if (mVisualElement == MyVis::LENGTH){
		MyQuarternion orientation(-MY_PI / 2, 0, 0, 1);
		MyGraphDrawer::MultiplyMatrix(&(orientation.GetMatrix().Transpose()));
	}
	mMapper.SetRandomSeed(i);
	mMapper.DrawValueMarker(mNodeValues[i]);
	MyGraphDrawer::PopMatrix();

	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

//#include <algorithm>

void MyGraphMatrixEncodingRepresentation::ShowEdge(int i){
	MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(MyGenericGraphf::EncodeEdge(idx2));
	//MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetEdgeWeight(i)]);
	/*
	// for gen pictures
	float weight = mLayout->GetGraph()->GetEdgeWeight(i);
	std::vector<float> weights;
	for (int ie = 0; ie < mLayout->GetGraph()->GetNumEdges(); ie++){
		weights.push_back(mLayout->GetGraph()->GetEdgeWeight(ie));
	}
	std::sort(weights.begin(), weights.end());
	int idx = -1;
	for (int ie = mLayout->GetGraph()->GetNumEdges()-1; ie >= 0; ie--){
		if (weights[ie] == weight){
			idx = ie;
			break;
		}
	}
	float idxf = 1 - idx / (float)(mLayout->GetGraph()->GetNumEdges() - 1);
	if (idxf > 0.5) MyGraphDrawer::Color(MyColor4f(247 / 255.f, 247 / 255.f, 185 / 255.f, 1));
	else MyGraphDrawer::Color(MyColor4f(173 / 255.f, 221 / 255.f, 142 / 255.f, 1));
	//else MyGraphDrawer::Color(MyColor4f(49 / 255.f, 163 / 255.f, 84 / 255.f, 1));
	// for gen end
	*/
	//MyGraphDrawer::Color(MyColor4f(1,1,1,0.5));
	float x = mLayout->GetNodePos(idx2[0])[0];
	float y = mLayout->GetNodePos(idx2[1])[1];
	/*
	if(x<y){
		int t = x;
		x = y;
		y = t;
	}
	*/
	MyGraphDrawer::Color(GetEdgeDefaultColor());
	MyGraphDrawer::BeginQuads();
	MyGraphDrawer::Vertex(MyVec3f((x + 0.1)* mCellWidth, (y + 0.1)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((x + 0.9)* mCellWidth, (y + 0.1)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((x + 0.9)* mCellWidth, (y + 0.9)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((x + 0.1)* mCellWidth, (y + 0.9)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((y + 0.1)* mCellWidth, (x + 0.1)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((y + 0.9)* mCellWidth, (x + 0.1)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((y + 0.9)* mCellWidth, (x + 0.9)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((y + 0.1)* mCellWidth, (x + 0.9)* mCellHeight, 0));
	MyGraphDrawer::EndPrimitive();
	/*
	MyGraphDrawer::DrawQuadAt(
		MyVec3f((x + 0.1)* mCellWidth, (y + 0.1)* mCellHeight, 0),
		MyVec3f((x + 0.9)* mCellWidth, (y + 0.1)* mCellHeight, 0),
		MyVec3f((x + 0.9)* mCellWidth, (y + 0.9)* mCellHeight, 0),
		MyVec3f((x + 0.1)* mCellWidth, (y + 0.9)* mCellHeight, 0));
	
	MyGraphDrawer::DrawQuadAt(
		MyVec3f((y + 0.1)* mCellWidth, (x + 0.1)* mCellHeight, 0),
		MyVec3f((y + 0.9)* mCellWidth, (x + 0.1)* mCellHeight, 0),
		MyVec3f((y + 0.9)* mCellWidth, (x + 0.9)* mCellHeight, 0),
		MyVec3f((y + 0.1)* mCellWidth, (x + 0.9)* mCellHeight, 0));
	*/	
	//MyGraphDrawer::DrawQuadAt(MyVec3f(y,x,0), 
	//	MyVec3f(y,x+1,0),
	//	MyVec3f(y+1,x+1,0),
	//	MyVec3f(y+1,x,0));
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphMatrixEncodingRepresentation::ShowHighlightedNode(int i){
	float n = (float)mLayout->GetGraph()->GetNumNodes();
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(i);
	//draw node glyph
	//mMapper.SetHighlightMode(true);
	MyGraphDrawer::Color(MyColor4f::white());
	// draw marker and label on the top
	MyGraphDrawer::PushMatrix();
	MyGraphDrawer::Translate(MyVec3f((mLayout->GetNodePos(i)[1] + 0.5f) * mCellWidth,
		(n/2 + 1)*mCellHeight, 0));
	int xp = mLayout->GetNodePos(i)[0] + n / 2;
	if (xp % 2 == 1 && mVisualElement != MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(0, MARKERSCALE, 0));
	}
	if (mVisualElement == MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(0, -MARKERSCALE/2.f + MARKERBOARDER, 0));
	}
	MyGraphDrawer::Scale(MARKERSCALE);
	mMapper.SetRandomSeed(i);
	mMapper.SetHighlightColor(this->GetNodeLabelColor(i));
	mMapper.DrawValueMarker(mNodeValues[i]);
	MyGraphDrawer::PopMatrix();
	// draw marker and label on the right side
	MyGraphDrawer::PushMatrix();
	MyGraphDrawer::Translate(MyVec3f((n / 2 + 1) * mCellWidth,
		(mLayout->GetNodePos(i)[1] + 0.5f)*mCellHeight, 0));
	xp = mLayout->GetNodePos(i)[0] + n / 2;
	if (xp % 2 == 1 && mVisualElement != MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(MARKERSCALE, 0, 0));
	}
	if (mVisualElement == MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(-MARKERSCALE/2.f + MARKERBOARDER, 0, 0));
	}
	MyGraphDrawer::Scale(MARKERSCALE);
	if (mVisualElement == MyVis::LENGTH){
		MyQuarternion textOrientation(-MY_PI / 2, 0, 0, 1);
		MyGraphDrawer::MultiplyMatrix(&(textOrientation.GetMatrix().Transpose()));
	}
	mMapper.SetRandomSeed(i);
	mMapper.SetHighlightColor(this->GetNodeLabelColor(i));
	mMapper.DrawValueMarker(mNodeValues[i]);
	MyGraphDrawer::PopMatrix();
	//mMapper.SetHighlightMode(false);
	
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();

	// for gen pictures
	//return;
	// end node glyph
	//MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetNodeWeight(i)]);
	//MyGraphDrawer::Color(MyColor4f(1,0,0,0.5));
	MyGraphDrawer::Color(this->GetNodeLabelColor(i));
	MyGraphDrawer::SetLineWidth(1.f);
	MyGraphDrawer::WirePolygon();
	MyVec3f pos = mLayout->GetNodePos(i);
	float x = pos[0];
	float y = pos[1];
	if(mSelectionLevel == 0){
		if(this->DoNodeStaticHighlight(i)){
			if(mNodeHighlightByte & COLUMN){
				this->DrawColumnBox(x+n/2,x+n/2);
			}
		}
		this->DrawRowBox(x+n/2,x+n/2);
	}
	MyGraphDrawer::FillPolygon();
	MyGraphDrawer::SetLineWidth(1.0f);
}

void MyGraphMatrixEncodingRepresentation::ShowNodeHighlight(int i){
	// for gen pictures
	//return;

	float n = (float)mLayout->GetGraph()->GetNumNodes();
	
	// draw marker and label on the top side
	MyGraphDrawer::PushMatrix();
	MyGraphDrawer::Translate(MyVec3f((mLayout->GetNodePos(i)[1] + 0.5f) * mCellWidth,
		(n/2 + 1)*mCellHeight, 0));
	int xp = mLayout->GetNodePos(i)[0] + n / 2;
	if (xp % 2 == 1 && mVisualElement != MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(0, MARKERSCALE, 0));
	}
	if (mVisualElement == MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(0, -MARKERSCALE/2.f, 0));
	}
	MyGraphDrawer::Scale(MARKERSCALE);
	mMapper.SetRandomSeed(i);
	mMapper.SetHighlightColor(this->GetNodeLabelColor(i));
	mMapper.DrawHighlightBack(mNodeValues[i]);
	MyGraphDrawer::PopMatrix();

	// draw marker and label on the right side
	MyGraphDrawer::PushMatrix();
	MyGraphDrawer::Translate(MyVec3f((n / 2 + 1) * mCellWidth,
		(mLayout->GetNodePos(i)[1] + 0.5f)*mCellHeight, 0));
	xp = mLayout->GetNodePos(i)[0] + n / 2;
	if (xp % 2 == 1 && mVisualElement != MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(MARKERSCALE, 0, 0));
	}
	if (mVisualElement == MyVis::LENGTH){
		MyGraphDrawer::Translate(MyVec3f(-MARKERSCALE/2.f, 0, 0));
	}
	MyGraphDrawer::Scale(MARKERSCALE);
	if (mVisualElement == MyVis::LENGTH){
		MyQuarternion textOrientation(-MY_PI / 2, 0, 0, 1);
		MyGraphDrawer::MultiplyMatrix(&(textOrientation.GetMatrix().Transpose()));
	}
	mMapper.SetRandomSeed(i);
	mMapper.SetHighlightColor(this->GetNodeLabelColor(i));
	mMapper.DrawHighlightBack(mNodeValues[i]);
	MyGraphDrawer::PopMatrix();
}

void MyGraphMatrixEncodingRepresentation::ShowHighlightedEdge(int i){
	MyVec2i idx2 = mLayout->GetGraph()->GetEdge(i);
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(MyGenericGraphf::EncodeEdge(idx2));
	//MyGraphDrawer::Color(mColorScheme[mLayout->GetGraph()->GetEdgeWeight(i)]);
	float x = mLayout->GetNodePos(idx2[0])[0];
	float y = mLayout->GetNodePos(idx2[1])[1];
	
	MyColor4f highlightColor = this->GetEdgeColor(i);
	MyColor4f defaultColor = this->GetEdgeDefaultColor();
	int highlightIdx = 0;

	if(mSelectionLevel == 0){
		MyGraphDrawer::Color(highlightColor);
	}
	else{
		float n = mLayout->GetGraph()->GetNumNodes();
		if(this->DoNodeHighlight(idx2[0])){
			highlightIdx |= 0x01;
		}
		if(this->DoNodeHighlight(idx2[1])){
			highlightIdx |= 0x02;
		}

		if(highlightIdx & 0x02){
			MyGraphDrawer::Color(highlightColor);
		}
		else{
			MyGraphDrawer::Color(defaultColor);
		}
	}

	MyGraphDrawer::BeginQuads();
	MyGraphDrawer::Vertex(MyVec3f((x + 0.1)* mCellWidth, (y + 0.1)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((x + 0.9)* mCellWidth, (y + 0.1)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((x + 0.9)* mCellWidth, (y + 0.9)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((x + 0.1)* mCellWidth, (y + 0.9)* mCellHeight, 0));
	MyGraphDrawer::EndPrimitive();
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
	if(mSelectionLevel == 0){
		MyGraphDrawer::Color(highlightColor);
	}
	else{
		if(highlightIdx & 0x01){
			MyGraphDrawer::Color(highlightColor);
		}
		else{
			MyGraphDrawer::Color(defaultColor);
		}
	}

	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_EDGE);
	MyGraphDrawer::PushName(MyGenericGraphf::EncodeEdge(idx2));
	MyGraphDrawer::BeginQuads();
	MyGraphDrawer::Vertex(MyVec3f((y + 0.1)* mCellWidth, (x + 0.1)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((y + 0.9)* mCellWidth, (x + 0.1)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((y + 0.9)* mCellWidth, (x + 0.9)* mCellHeight, 0));
	MyGraphDrawer::Vertex(MyVec3f((y + 0.1)* mCellWidth, (x + 0.9)* mCellHeight, 0));
	MyGraphDrawer::EndPrimitive();
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphMatrixEncodingRepresentation::ShowLabel(int idx){
	bool showRightSideLabel = mLabelDrawByte&RIGHT;
	bool showTopSideLabel = mLabelDrawByte&TOP;
	float labelScale = 0.6;
	float scale = SINGLEGRAPHSIZE;
	MyGraphDrawer::Color(this->GetNodeLabelColor(idx));
	// right side
	if(showRightSideLabel){
		MyVec3f pos = this->GetNodeTangiblePos(idx,0);
		MyVec3f labelPos = pos;
		if (mVisualElement == MyVis::LENGTH){
			labelPos += MyVec3f(mMappingScale[mNodeValues[idx]] *  mMapper.GetMaxLength()*MARKERSCALE, 0, 0);
		}
		else{
			labelPos += MyVec3f(4 * mCellWidth, 0, 0);
		}
		this->DrawSelectableLabel(idx, labelPos+MyVec3f(0,-0.5,0),labelScale/scale);
	}

	// top side
	if(showTopSideLabel){
		MyVec3f pos = this->GetNodeTangiblePos(idx,1);
		MyVec3f labelPos = pos+MyVec3f(0,mLabelObjects.GetTextObject(idx).GetWidth()/(scale/labelScale),0);
		if (mVisualElement == MyVis::LENGTH){
			labelPos += MyVec3f(0, mMappingScale[mNodeValues[idx]] *  mMapper.GetMaxLength()*MARKERSCALE, 0);
		}
		else{
			labelPos += MyVec3f(0, 2*MARKERSCALE, 0);
		}
		MyQuarternion rotate(-MY_PI/2,0,0,1);
		this->DrawSelectableLabel(idx, labelPos+MyVec3f(0,0,0),labelScale/scale, rotate);
	}
}

void MyGraphMatrixEncodingRepresentation::ShowLabels(){
	//MyGraphDrawer::PushMatrix();
	//MyGraphDrawer::PushProjectionMatrix();
	//MyMatrixf identityMat = MyMatrixf::IdentityMatrix();
	//MyMatrixf viewMat = MyMatrixf::OrthographicMatrix(0,1024,0,768,1,10000);
	//MyGraphDrawer::LoadModelViewMatrix(&identityMat);
	//MyGraphDrawer::LoadProjectionMatrix(&identityMat);
	//mLabelManager.Render();
	//MyGraphDrawer::PopMatrix();
	//MyGraphDrawer::PopProjectionMatrix();
	
	MyMatrixd mvmat = MyGraphDrawer::GetModelViewMatrix();
	float scale = mvmat.At(1,1);
	float n = mLayout->GetGraph()->GetNumNodes();
	MyGraphDrawer::Color(MyColor4f(0.f,0.f,0.f,1.f));
	for(int i = 0; i < n; ++i){
		MyGraphDrawer::Color(this->GetNodeLabelColor(i));
		// right side
		MyVec3f pos = this->GetNodeTangiblePos(i,0);
		MyVec3f labelPos = pos;
		if (mVisualElement == MyVis::LENGTH){
			labelPos += MyVec3f(mMappingScale[mNodeValues[i]] *  mMapper.GetMaxLength()*4+2, 0, 0);
		}
		else{
			labelPos += MyVec3f(4 * mCellWidth, 0, 0);
		}
		this->DrawSelectableLabel(i, labelPos+MyVec3f(0,-0.5,0),0.75/scale);
	}
	
	/*
	for(int i = 0; i < n; ++i){
		MyGraphDrawer::Color(this->GetNodeLabelColor(i));
		// top
		MyVec3f pos = this->GetNodeTangiblePos(i,1);
		MyVec3f labelPos = pos+MyVec3f(0,mLabelObjects.GetTextObject(i).GetWidth()/scale,0);
		if (mVisualElement == MyVis::LENGTH){
			labelPos += MyVec3f(0, mMappingScale[mNodeValues[i]] *  mMapper.GetMaxLength()*4+2, 0);
		}
		else{
			labelPos += MyVec3f(0, 4 * mCellWidth, 0);
		}
		MyQuarternion rotate(-MY_PI/2,0,0,1);
		this->DrawSelectableLabel(i, labelPos+MyVec3f(0,-0.5,0),0.75/scale, rotate);
	}
	*/
}

MyVec3f MyGraphMatrixEncodingRepresentation::GetNodeTangiblePos(int i, int mIdx) const{
	float n = mLayout->GetGraph()->GetNumNodes();
	if(mIdx == 1){
		// top
		MyVec3f pos = MyVec3f((mLayout->GetNodePos(i)[1] + 0.5f) * mCellWidth,
			(n/2 + 1)*mCellHeight, 0);
		int xp = mLayout->GetNodePos(i)[0] + n / 2;
		if (xp % 2 == 1 && mVisualElement != MyVis::LENGTH){
			pos += MyVec3f(0, 4, 0);
		}
		return pos;
	}
	else{
		MyVec3f pos = MyVec3f((n / 2 + 1) * mCellWidth,
		(mLayout->GetNodePos(i)[1] + 0.5f)*mCellHeight, 0);
		int xp = mLayout->GetNodePos(i)[0] + n / 2;
		if (xp % 2 == 1 && mVisualElement != MyVis::LENGTH){
			pos += MyVec3f(4, 0, 0);
		}
		return pos;
	}
}

int MyGraphMatrixEncodingRepresentation::GetNumOfTangibleMarker(int i) const{
	return 2;
}

MyBoundingBox MyGraphMatrixEncodingRepresentation::GetNodeTangibleBox(int i, int mIdx) const{
	/*
	MyVec3f pos = this->GetNodeTangiblePos(i,mIdx);
	MyBoundingBox box(pos, pos);
	// default the marker size is 1
	box.expand(2,0);
	box.expand(2,1);
	box.expand(2,2);
	return box;
	*/
	
	float n = (float)mLayout->GetGraph()->GetNumNodes();
	MyVec3f pos = mLayout->GetNodePos(i);
	float x = pos[0];
	if(mIdx == 0){
		// horizontal
		MyVec3f leftBottom(-n / 2 * mCellWidth,  x*mCellHeight+mCellBoarder_Y, 0.f);
		MyVec3f rightTop(n / 2 * mCellWidth,  (x+1)*mCellHeight-mCellBoarder_Y, 0.f);
		return MyBoundingBox(leftBottom,rightTop);
	}
	else{
		//vertical
		MyVec3f leftBottom(x * mCellWidth+mCellBoarder_X,  -n / 2 * mCellHeight, 0.f);
		MyVec3f rightTop((x+1) * mCellWidth-mCellBoarder_X,  n / 2 * mCellHeight, 0.f);
		return MyBoundingBox(leftBottom,rightTop);
	}
}

void MyGraphMatrixEncodingRepresentation::DrawRowBox(int startRow, int endRow){
	float n = (float)mLayout->GetGraph()->GetNumNodes();
	MyVec3f leftBottom(-n / 2 * mCellWidth,  startRow*mCellHeight+mCellBoarder_Y, 0.f);
	MyVec3f rightBottom(n / 2 * mCellWidth,  startRow*mCellHeight+mCellBoarder_Y, 0.f);
	MyVec3f leftTop(-n / 2 * mCellWidth,  (endRow+1)*mCellHeight-mCellBoarder_Y, 0.f);
	MyVec3f rightTop(n / 2 * mCellWidth,  (endRow+1)*mCellHeight-mCellBoarder_Y, 0.f);

	MyVec3f offset(0,-n/2*mCellHeight,0);
	MyGraphDrawer::SetLineWidth(1);
	MyGraphDrawer::BeginLineLoop();
	MyGraphDrawer::Vertex(leftBottom+offset);
	MyGraphDrawer::Vertex(rightBottom+offset);
	MyGraphDrawer::Vertex(rightTop+offset);
	MyGraphDrawer::Vertex(leftTop+offset);
	MyGraphDrawer::EndPrimitive();
}

void MyGraphMatrixEncodingRepresentation::DrawColumnBox(int startCol, int endCol){
	float n = (float)mLayout->GetGraph()->GetNumNodes();
	MyVec3f leftBottom(startCol * mCellWidth+mCellBoarder_X,  -n / 2 * mCellHeight, 0.f);
	MyVec3f rightBottom((endCol+1) * mCellWidth-mCellBoarder_X,  -n / 2 * mCellHeight, 0.f);
	MyVec3f leftTop(startCol * mCellWidth+mCellBoarder_X,  n / 2 * mCellHeight, 0.f);
	MyVec3f rightTop((endCol+1) * mCellWidth-mCellBoarder_X,  n / 2 * mCellHeight, 0.f);

	MyVec3f offset(-n/2*mCellWidth,0,0);
	MyGraphDrawer::SetLineWidth(1);
	MyGraphDrawer::BeginLineLoop();
	MyGraphDrawer::Vertex(leftBottom+offset);
	MyGraphDrawer::Vertex(rightBottom+offset);
	MyGraphDrawer::Vertex(rightTop+offset);
	MyGraphDrawer::Vertex(leftTop+offset);
	MyGraphDrawer::EndPrimitive();
}

void MyGraphMatrixEncodingRepresentation::SetLabelDraw(int drawByte){
	mLabelDrawByte = drawByte;
}

int MyGraphMatrixEncodingRepresentation::GetLabelDraw() const{
	return mLabelDrawByte;
}

void MyGraphMatrixEncodingRepresentation::SetNodeHighlightByte(int byte){
	mNodeHighlightByte = byte;
}

int MyGraphMatrixEncodingRepresentation::GetNodeHighlightByte() const{
	return mNodeHighlightByte;
}

void MyGraphMatrixEncodingRepresentation::SetNodeAffectEdgeHighlightByte(int byte){
	mNodeAffectEdgeHighlightByte = byte;
}

int MyGraphMatrixEncodingRepresentation::GetNodeAffectEdgeHighlightByte() const{
	return mNodeAffectEdgeHighlightByte;
}