#include "MyGraphEncodingRepresentation.h"
#include "MyGraphDrawer.h"
#include "MyVisualElement.h"
#include "MyColorConverter.h"
#include "MyUtility.h"
#include "My2DCellFillingAlgorithm.h"
#include "VisConfig.h"

//tmp sol
#include "MyGraphRadialLayout.h"
#include "MyGraphMatrixLayout.h"

MyGraphEncodingRepresentation::MyGraphEncodingRepresentation(void)
{
	mValueRangeSet = false;
}


MyGraphEncodingRepresentation::~MyGraphEncodingRepresentation(void)
{
}

void MyGraphEncodingRepresentation::Show(){
	if(mVisualElement == MyVis::LENGTH
		&& dynamic_cast<const MyGraphRadialLayout*>(mLayout)!=0){
		float radius = mLayout->GetNodePos(0).norm();
		float steps = 5;
		float incr = mMapper.GetMaxLength()*MARKERSCALE/steps;
		MyGraphDrawer::Color(MyColor4f(0.6,0.6,0.6,0.6));
		for(int i = 0;i<steps;i++){
			MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(),radius+i*incr,100);
		}
	}
	/*
	if(dynamic_cast<const MyGraphRadialLayout*>(mLayout)==0){
		float buttom = mLayout->GetBoundingBox().GetBottom();
		float top = mLayout->GetBoundingBox().GetTop();
		MyGraphDrawer::Color(MyColor4f(0.6f, 0.6f, 0.6f, 1.f));
		MyPrimitiveDrawer::DrawLineAt(MyVec3f(0,buttom,0),
			MyVec3f(0,top,0));
	}
	*/
	MyGraphRepresentation::Show();
}

void MyGraphEncodingRepresentation::Update(){
	MyGraphRepresentation::Update();
	// change the color scale
	MyArrayi *sortInfo = mNodeValues.MakeSortResultArray();
	float minValue = mNodeValues[sortInfo->front()];
	float maxValue = mNodeValues[sortInfo->back()];
	mColorScheme.SetDomain(minValue, maxValue);
	delete sortInfo;
	MyArray<MyColor4f> colors;
	for (int i = 0; i <= 20; i++){
	MyColor4f color;
	//rgba rgb = MyColorConverter::lab2rgb(i/20.f * 100, 1, 1);
	rgba rgb = MyColorConverter::lab2rgb(100, (1-i/20.f)*500, i/20.f*200);
	color.r = rgb.r;
	color.g = rgb.g;
	color.b = rgb.b;
	colors << color;
	}
	mColorScheme.SetRange(colors);
	this->UpdataLabel();
}

void MyGraphEncodingRepresentation::ShowNode(int i){
	MyGraphDrawer::PushName(MyGenericGraphf::GRAPH_NODE);
	MyGraphDrawer::PushName(i);
	drawEncodedNode(i);
	MyGraphDrawer::PopName();
	MyGraphDrawer::PopName();
}

void MyGraphEncodingRepresentation::ShowHighlightedNode(int i){
	// set highlight mode if a colored outline is needed
	mMapper.SetHighlightMode(true);
	this->ShowNode(i);
	mMapper.SetHighlightMode(false);
}

void MyGraphEncodingRepresentation::ShowNodeHighlight(int i){
	for(int im = 0;im<this->GetNumOfTangibleMarker(i);im++){
		MyVec3f pos = this->GetNodeTangiblePos(i, im);
		MyGraphDrawer::PushMatrix();
		MyGraphDrawer::Translate(pos);
		if (mVisualElement == MyVis::LENGTH){
			MyGraphDrawer::MultiplyMatrix(&(mLayout->GetNodeOrientation(i).GetMatrix().Transpose()));
		}
		MyGraphDrawer::Scale(MARKERSCALE);
		mMapper.DrawHighlightBack(mNodeValues[i]);
		MyGraphDrawer::PopMatrix();
	}
}

void MyGraphEncodingRepresentation::SetNodeValuef(MyArrayf values){
	mNodeValues = values;
	if(!mValueRangeSet){
		MyArrayi *sortInfo = values.MakeSortResultArray();
		mMappingScale.SetDomain(values[sortInfo->front()],values[sortInfo->back()]);
		delete sortInfo;
	}
	mMappingScale.SetRange(0,1);
	mMapper.SetScale(mMappingScale);
}

void MyGraphEncodingRepresentation::SetNodeEncodingMethods(MyVis::MyVisualElement ele){
	mVisualElement = ele;
	mMapper.SetVisualElement(ele);
}

void MyGraphEncodingRepresentation::drawEncodedNode(int i){
	MyGraphDrawer::PushMatrix();
	MyGraphDrawer::Translate(mLayout->GetNodePos(i));
	if (mVisualElement == MyVis::LENGTH){
		MyGraphDrawer::MultiplyMatrix(&(mLayout->GetNodeOrientation(i).GetMatrix().Transpose()));
	}
	MyGraphDrawer::Scale(MARKERSCALE);
	mMapper.SetRandomSeed(i);
	mMapper.SetHighlightColor(this->GetNodeLabelColor(i));
	mMapper.DrawValueMarker(mNodeValues[i]);
	MyGraphDrawer::PopMatrix();
}

void MyGraphEncodingRepresentation::SetValueRange(float min, float max){
	mMappingScale.SetDomain(min,max);
	mMappingScale.SetRange(0,1);
	mMapper.SetScale(mMappingScale);
	mValueRangeSet = true;
}

const MyScale<float, MyColor4f>& MyGraphEncodingRepresentation::GetColorScale() const{
	if(mVisualElement == MyVis::SATURATION){
		return mMapper.GetSaturationScale();
	}
	else{
		return mMapper.GetHueScale();
	}
}

const MyVisualElementSingleValueMapper& MyGraphEncodingRepresentation::GetMapper() const{
	return mMapper;
}; 

const MyScale<float, float>& MyGraphEncodingRepresentation::GetMappingScale() const{
	return mMappingScale;
}

MyVis::MyVisualElement MyGraphEncodingRepresentation::GetVisualElement() const{
	return mVisualElement;
}

void MyGraphEncodingRepresentation::DrawMarker(float value, int randomSeed){
	mMapper.SetRandomSeed(randomSeed);
	mMapper.DrawValueMarker(value);
}

void MyGraphEncodingRepresentation::DrawLegend(){
	// a vertical one
	/*
	MyBoundingBox box(MyVec3f(0,0,0),MyVec3f(10,60,0));
	int numSegs = 20;
	int numTicks = 6;
	float minValue = mMappingScale.GetDomain()[0];
	float maxValue = mMappingScale.GetDomain()[1];

	if(mVisualElement == MyVis::SATURATION ||
		mVisualElement == MyVis::HUE){
		// draw the color bar
		const MyScale<float, MyColor4f>& colorScale = this->GetColorScale();
		
		float cellHeight = box.GetHeight()/(numSegs-1);
		float cellLeft = box.GetLeft();
		float cellRight = box.GetRight();
		float cellBottom = box.GetBottom();
		MyPrimitiveDrawer::BeginQuadStrip();
		for(int i = 0;i<numSegs;i++){
			MyColor4f iColor = colorScale[i/(float)numSegs];
			MyPrimitiveDrawer::Color(iColor);
			MyPrimitiveDrawer::Vertex(MyVec3f(cellLeft,cellBottom+i*cellHeight,0));
			MyPrimitiveDrawer::Vertex(MyVec3f(cellRight,cellBottom+i*cellHeight,0));
		}
		MyPrimitiveDrawer::EndPrimitive();

		MyPrimitiveDrawer::WirePolygon();
		MyPrimitiveDrawer::Color(MyColor4f::white());
		MyPrimitiveDrawer::BeginQuads();
		MyPrimitiveDrawer::Vertex(MyVec3f(box.GetLeft(), box.GetBottom(),0));
		MyPrimitiveDrawer::Vertex(MyVec3f(box.GetRight(), box.GetBottom(),0));
		MyPrimitiveDrawer::Vertex(MyVec3f(box.GetRight(), box.GetTop(),0));
		MyPrimitiveDrawer::Vertex(MyVec3f(box.GetLeft(), box.GetTop(),0));
		MyPrimitiveDrawer::EndPrimitive();
		MyPrimitiveDrawer::FillPolygon();
	}
	else{
		float cellHeight = box.GetHeight()/(numTicks);
		float cellCenter = (box.GetLeft()+box.GetRight())/2;
		float cellBottom = box.GetBottom();
		float sizeScale = 4;
		for(int i = 0;i<=numTicks;i++){
			float value = minValue+(maxValue-minValue)/numTicks*i;
			MyPrimitiveDrawer::PushMatrix();
			MyPrimitiveDrawer::Translate(MyVec3f(cellCenter,cellBottom+i*cellHeight,0));
			MyPrimitiveDrawer::Scale(sizeScale);
			if(mVisualElement == MyVis::LENGTH){
				MyQuarternion rotate(MY_PI/2,0,0,1);
				MyPrimitiveDrawer::MultiplyMatrix(&(rotate.GetMatrix().Transpose()));
				mMapper.DrawValueMarker(value);
			}
			else mMapper.DrawValueMarker(value);
			MyPrimitiveDrawer::PopMatrix();
		}
	}

	// draw text ticks
	MyPrimitiveDrawer::Color(MyColor4f::white());
	for(int i = 0;i<=numTicks;i++){
		float cellHeight = box.GetHeight()/(numTicks);
		MyVec3f bl(box.GetRight()+0.15,box.GetBottom()+i*cellHeight-0.5,0);
		float value = minValue+(maxValue-minValue)/numTicks*i;
		MyString tick(value);
		tick.resize(5);
		MyPrimitiveDrawer::DrawBitMapText(bl,tick);
		if(mVisualElement == MyVis::SATURATION ||
			mVisualElement == MyVis::HUE){
			MyPrimitiveDrawer::BeginLines();
			MyPrimitiveDrawer::Vertex(MyVec3f(5,box.GetBottom()+i*cellHeight,0));
			MyPrimitiveDrawer::Vertex(MyVec3f(10.0,box.GetBottom()+i*cellHeight,0));
			MyPrimitiveDrawer::EndPrimitive();
		}
	}
	*/

	/*

		a horizontal one

	*/

	MyBoundingBox box(MyVec3f(0,0,0),MyVec3f(160,MARKERSCALE,0));
	int numSegs = 20;
	int numTicks = 6;
	float minValue = mMappingScale.GetDomain()[0];
	float maxValue = mMappingScale.GetDomain()[1];

	if(mVisualElement == MyVis::SATURATION ||
		mVisualElement == MyVis::HUE){
		// draw the color bar
		const MyScale<float, MyColor4f>& colorScale = this->GetColorScale();
		
		float cellWidth = box.GetWidth()/(numSegs-1);
		float cellLeft = box.GetLeft();
		float cellRight = box.GetRight();
		float cellBottom = box.GetBottom();
		float cellTop = box.GetTop();
		MyPrimitiveDrawer::BeginQuadStrip();
		for(int i = 0;i<numSegs;i++){
			MyColor4f iColor = colorScale[i/(float)numSegs];
			MyPrimitiveDrawer::Color(iColor);
			MyPrimitiveDrawer::Vertex(MyVec3f(cellLeft+i*cellWidth,cellBottom,0));
			MyPrimitiveDrawer::Vertex(MyVec3f(cellLeft+i*cellWidth,cellTop,0));
		}
		MyPrimitiveDrawer::EndPrimitive();

		MyPrimitiveDrawer::WirePolygon();
		if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
		else MyPrimitiveDrawer::Color(MyColor4f::white());
		MyPrimitiveDrawer::BeginQuads();
		MyPrimitiveDrawer::Vertex(MyVec3f(box.GetLeft(), box.GetBottom(),0));
		MyPrimitiveDrawer::Vertex(MyVec3f(box.GetRight(), box.GetBottom(),0));
		MyPrimitiveDrawer::Vertex(MyVec3f(box.GetRight(), box.GetTop(),0));
		MyPrimitiveDrawer::Vertex(MyVec3f(box.GetLeft(), box.GetTop(),0));
		MyPrimitiveDrawer::EndPrimitive();
		MyPrimitiveDrawer::FillPolygon();
	}
	else{
		float cellWidth = box.GetWidth()/(numTicks);
		float cellCenter = (box.GetBottom()+box.GetTop())/2;
		if(mVisualElement == MyVis::LENGTH){
			cellCenter = box.GetBottom();
		}
		float cellBottom = box.GetBottom();
		float cellLeft = box.GetLeft();
		float cellTop = box.GetTop();
		for(int i = 0;i<=numTicks;i++){
			float value = minValue+(maxValue-minValue)/numTicks*i;
			MyPrimitiveDrawer::PushMatrix();
			MyPrimitiveDrawer::Translate(MyVec3f(cellLeft+i*cellWidth,cellCenter,0));
			MyPrimitiveDrawer::Scale(MARKERSCALE);
			mMapper.SetRandomSeed(i);
			mMapper.DrawValueMarker(value);
			MyPrimitiveDrawer::PopMatrix();
		}
	}

	// draw text ticks
	//MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::Color(MyColor4f::black());
	for(int i = 0;i<=numTicks;i++){
		float cellWidth = box.GetWidth()/(numTicks);
		MyVec3f bl(box.GetLeft()-0.05+i*cellWidth,box.GetBottom()-2,0);
		float value = minValue+(maxValue-minValue)/numTicks*i;
		MyString tick(value);
		tick.resize(5);
		MyPrimitiveDrawer::DrawBitMapText(bl,tick,1);
		if(mVisualElement == MyVis::SATURATION ||
			mVisualElement == MyVis::HUE){
			MyPrimitiveDrawer::BeginLines();
			MyPrimitiveDrawer::Vertex(MyVec3f(i*cellWidth,box.GetBottom(),0));
			MyPrimitiveDrawer::Vertex(MyVec3f(i*cellWidth,box.GetBottom()+0.5*box.GetHeight(),0));
			MyPrimitiveDrawer::EndPrimitive();
		}
	}
}

void MyGraphEncodingRepresentation::UpdataLabel(){
	MyGraphRepresentation::UpdataLabel();
	if( dynamic_cast<const MyGraphRadialLayout*>(mLayout)==0
		&&dynamic_cast<const MyGraphMatrixLayout*>(mLayout)==0){
		
		// init grid
		MyBoundingBox gridbox = mLayout->GetBoundingBox();
		MySimple2DGrid grid;
		grid.SetRange(gridbox.GetLowPos().toDim<2>(), gridbox.GetHighPos().toDim<2>());
		grid.SetXDivision(200);
		grid.SetYDivision(200);
		My2DCellFillingAlgorithm algor;
		algor.SetGrid(grid);

		// fill in markers
		for(int i = 0; i < mLayout->GetGraph()->GetNumNodes(); i++){
			MyBox2f box = mMapper.GetMarkerBox(mNodeValues[i])*MARKERSCALE/2;
			if(mVisualElement == MyVis::LENGTH) box *= 2;
			//MyBox2f box(-MyVec2f(MARKERSCALE/4, MARKERSCALE/4),MyVec2f(MARKERSCALE/4, MARKERSCALE/4));
			box += mLayout->GetNodePos(i).toDim<2>();
			algor.PutObject(box);
		}

		for(int i = 0; i < mLayout->GetGraph()->GetNumNodes(); i++){
			mLabelPos[i] += MyVec3f(2,0,0);
		}
		// fill in labels
		// static highlight first
		
		for(int i = 0;i<mStaticSelectedNodes.size();i++){
			int nodeIdx = mStaticSelectedNodes[i];
			float width = mLabelObjects.GetTextObject(nodeIdx).GetWidth();
			float height = mLabelObjects.GetTextObject(nodeIdx).GetHeight();
			MyBox2f box(MyVec2f::zero(),MyVec2f(width, height)/SINGLEGRAPHSIZE);
			box-=MyVec2f(0,height/SINGLEGRAPHSIZE/2);
			box += mLabelPos[nodeIdx].toDim<2>();
			MyVec2f anchor = mLayout->GetNodePos(nodeIdx).toDim<2>();
			if(algor.CanPutObject(box)){
				algor.PutObject(box);
			}
			else if(algor.TryPutObjectNear(box, anchor, 10000)){
				mLabelPos[nodeIdx] = (2*anchor-box).GetLowPos().toDim<3>(0) 
					+ MyVec3f(0,height/SINGLEGRAPHSIZE/2,0);
			}
			else{
				algor.PutObject(box);
			}
		}
		for(int i = 0;i<mSelectedNodes.size();i++){
			int nodeIdx = mSelectedNodes[i];
			if(mStaticSelectedNodes.HasOne(nodeIdx)) continue;
			float width = mLabelObjects.GetTextObject(nodeIdx).GetWidth();
			float height = mLabelObjects.GetTextObject(nodeIdx).GetHeight();
			MyBox2f box(MyVec2f::zero(),MyVec2f(width, height)/SINGLEGRAPHSIZE);
			box-=MyVec2f(0,height/SINGLEGRAPHSIZE/2);
			box += mLabelPos[nodeIdx].toDim<2>();
			MyVec2f anchor = mLayout->GetNodePos(nodeIdx).toDim<2>();
			if(algor.CanPutObject(box)){
				algor.PutObject(box);
			}
			else if(algor.TryPutObjectNear(box, anchor, 10000)){
				mLabelPos[nodeIdx] = (2*anchor-box).GetLowPos().toDim<3>(0) 
					+ MyVec3f(0,height/SINGLEGRAPHSIZE/2,0);
			}
			else{
				algor.PutObject(box);
			}
		}
		
		
		for(int i = 0; i < mLayout->GetGraph()->GetNumNodes(); i++){
			if(mStaticSelectedNodes.HasOne(i)) continue;
			if(mSelectedNodes.HasOne(i)) continue;
			float width = mLabelObjects.GetTextObject(i).GetWidth();
			float height = mLabelObjects.GetTextObject(i).GetHeight();
			MyBox2f box(MyVec2f::zero(),MyVec2f(width, height)/SINGLEGRAPHSIZE);
			box-=MyVec2f(0,height/SINGLEGRAPHSIZE/2);
			box += mLabelPos[i].toDim<2>();
			MyVec2f anchor = mLayout->GetNodePos(i).toDim<2>();
			if(algor.CanPutObject(box)){
				algor.PutObject(box);
			}
			else if(algor.TryPutObjectNear(box, anchor, 10000)){
				mLabelPos[i] = (2*anchor-box).GetLowPos().toDim<3>(0) 
					+ MyVec3f(0,height/SINGLEGRAPHSIZE/2,0);
			}
			else{
				mLabelShow[i] = false;
			}
		}
		
		mFillMatrix = algor.GetFillMatrix();
	}
}

void MyGraphEncodingRepresentation::ShowLabel(int idx){
	float scale = SINGLEGRAPHSIZE;
	if( dynamic_cast<const MyGraphRadialLayout*>(mLayout)==0){
		MyGraphDrawer::Color(this->GetNodeLabelColor(idx));
		this->DrawSelectableLabel(idx, mLabelPos[idx],1/scale);
	}
	else{
		MyGraphDrawer::Color(this->GetNodeLabelColor(idx));
		MyVec3f pos = mLayout->GetNodePos(idx);
			
		float tangle = atan2(pos[1], fabs(pos[0]));
		MyVec3f labelPos = pos + pos.normalized()*MARKERSCALE/2;
		if (mVisualElement == MyVis::LENGTH){
			labelPos += pos.normalized()*mMappingScale[mNodeValues[idx]] *  mMapper.GetMaxLength()*MARKERSCALE;
		}
		//labelPos[1] = tangle / MY_PI * 2 * 80;
		//MyGraphDrawer::Translate(labelPos);
		MyQuarternion orientation = mLayout->GetNodeOrientation(idx);
		MyQuarternion textOrientationModifier(MY_PI/2,0,0,1);
		orientation = textOrientationModifier*orientation;
		//MyGraphDrawer::MultiplyMatrix(&(textOrientation2.GetMatrix().Transpose()));
		float angle = acos(orientation[0])*2;
		if(angle<=0) angle+=MY_PI*2;
		if(angle>=MY_PI/2 && angle<MY_PI/2*3){
			MyQuarternion upsideDown(MY_PI,0,0,1);
			orientation = upsideDown*orientation;
			labelPos += pos.normalized()*mLabelObjects.GetTextObject(idx).GetWidth()/scale;
			this->DrawSelectableLabel(idx, labelPos, 1/scale,orientation);
		}
		else{
			this->DrawSelectableLabel(idx, labelPos, 1/scale,orientation);
		}
	}

}

void MyGraphEncodingRepresentation::ShowLabels(){

	if( dynamic_cast<const MyGraphRadialLayout*>(mLayout)==0){

		MyGraphDrawer::EnableAlplaBlending();
		MyVec3f start = mLayout->GetBoundingBox().GetLowPos();
		start[2] = 0;
		float cellWidth = mLayout->GetBoundingBox().GetWidth()/mFillMatrix.GetNumCols();
		float cellHeight = mLayout->GetBoundingBox().GetHeight()/mFillMatrix.GetNumRows();
		for(int i = 0;i<mFillMatrix.GetNumCols();i++){
			for(int j = 0;j<mFillMatrix.GetNumRows();j++){
				int fill = mFillMatrix.At(i,j);
				if(fill == 1){
					MyGraphDrawer::Color(MyColor4f(1,0,0,0.4));
				}
				else{
					MyGraphDrawer::Color(MyColor4f(0,0,0,0));
				}
				

				MyGraphDrawer::BeginQuads();
				MyGraphDrawer::Vertex(start+MyVec3f(i*cellWidth, j*cellHeight,0));
				MyGraphDrawer::Vertex(start+MyVec3f((i+1)*cellWidth, j*cellHeight,0));
				MyGraphDrawer::Vertex(start+MyVec3f((i+1)*cellWidth, (j+1)*cellHeight,0));
				MyGraphDrawer::Vertex(start+MyVec3f(i*cellWidth, (j+1)*cellHeight,0));
				MyGraphDrawer::EndPrimitive();
			}
		}
		MyGraphDrawer::DisableAlplaBlending();
		
	}
	else{
		for(int i = 0; i < mLayout->GetGraph()->GetNumNodes(); ++i){

		}

	}
}

MyVec3f MyGraphEncodingRepresentation::GetNodeTangiblePos(int i, int mIdx) const{
	return mLayout->GetNodePos(i);
}

int MyGraphEncodingRepresentation::GetNumOfTangibleMarker(int i) const{
	return 1;
}

MyBoundingBox MyGraphEncodingRepresentation::GetNodeTangibleBox(int i, int mIdx) const{
	MyVec3f pos = this->GetNodeTangiblePos(i, mIdx);
	MyBoundingBox box(pos, pos);
	box.expand(MARKERSCALE/2,0);
	box.expand(MARKERSCALE/2,1);
	box.expand(MARKERSCALE/2,2);
	return box;
}