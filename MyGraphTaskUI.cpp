#include "MyGraphTaskUI.h"
#include "MyPrimitiveDrawer.h"
#include "MyGraphEncodingRepresentation.h"
#include "MyUtility.h"
#include "MyGenericHierarchyOverlay.h"
#include "MySelectionEvent.h"
#include "VisConfig.h"
#include <ctime>

// all from [-1,-1]~[1,1]
#define UI_LEFT 0.9
#define UI_BOTTOM -0.5
#define UI_BOX_HEIGHT 0.1
#define UI_BOX_WIDTH 0.08
#define UI_BOX_INTERVAL 0.02
#define UI_BOX_TEXT_LEFT_OFFSET 0.01
#define UI_BOX_TEXT_BOTTOM_OFFSET 0.04

// index
#define UI_FINISH_BOX_BOTTOM -0.8
#define UI_LEGEND_BOX_INDEX 9998
#define UI_FINISH_BOX_INDEX 9999
#define UI_JUMP_BOX_INDEX 9997
#define UI_PAUSE_BOX_INDEX 9996
#define UI_CANCEL_BOX_INDEX 9995

// cancel
#define UI_CANCEL_BOX_BOTTOM 0.70

// result just below finish box
#define UI_RESULT_LEFT 0.9
#define UI_RESULT_BOTTOM -0.55
#define UI_JUMP_BOX_BOTTOM -0.6

// the pause botton
#define UI_PAUSE_BOX_BOTTOM 0.85
// change resolution from 1200 to 1080
#define UI_DESCRIPTION_BOTTOM -0.98
//#define UI_DESCRIPTION_BOTTOM -0.84
#define UI_WARNING_BOTTOM 0

// legend uses world coordinates
#define UI_LEGEND_LEFT -460
#define UI_LEGEND_RIGHT 820
// change resolution from 1200 to 1080
#define UI_LEGEND_BOTTOM -520
//#define UI_LEGEND_BOTTOM -500
#define UI_LEGEND_TOP 0.5
#define UI_LEGEND_TEXT_SPACE 0.01
#define UI_LEGEND_TEXT_SIZE 0.03

#define UI_IDX_LEFT -0.95
#define UI_IDX_BOTTOM 0.95

#define UI_SELECTION_NAME_LEFT 0
#define UI_SELECTION_NAME_BOTTOM 0.8

#define UI_SCENE_DEPTH 630

int MyGraphTaskUI::mBackground = 0;

MyGraphTaskUI::MyGraphTaskUI(void)
{
	mHighlightedIndex = -1;
	mTickValue = 0.5f;
	mTickSet = false;
	mShowResult = false;
	mBeingDrag = false;
	mJumpAllowed = false;
	mFinished = false;
	mIsPaused = false;
	mGraphKnot = 0;
	mTask = 0;
	mGraphKnot = 0;
	mFinishBoxString = "NEXT";
	mUserSelection = -1;
	mEnabled = false;
}


MyGraphTaskUI::~MyGraphTaskUI(void)
{
}

void MyGraphTaskUI::SetEnable(bool b){
	mEnabled = b;
}

void MyGraphTaskUI::SetTask(MyGraphChoiceTask* task){
	mTask = task;
}


void MyGraphTaskUI::SetGraphKnot(MyGraphKnot* knot){
	mGraphKnot = knot;
}

void MyGraphTaskUI::Show(){
	
	// draw legend
	if (mGraphKnot){
		MyGraphEncodingRepresentation* rep = dynamic_cast<MyGraphEncodingRepresentation*>(mGraphKnot->GetRepresentation());
		MyPrimitiveDrawer::PushMatrix();
		static float dedede = 0;
		dedede+=0.01;
		MyPrimitiveDrawer::Translate(MyVec3f(UI_LEGEND_LEFT,UI_LEGEND_BOTTOM,-UI_SCENE_DEPTH));
		//MyPrimitiveDrawer::Scale(0.1*MyVec3f(1080/1920.f,1,1));
		if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
		else MyPrimitiveDrawer::Color(MyColor4f::white());
		MyPrimitiveDrawer::DrawBitMapTextLarge(MyVec3f(-120,0,0),MyVis::GetVisualElementName(rep->GetVisualElement()));
		MyPrimitiveDrawer::Color(MyColor4f(1,0,0,1));
		if(mTask->GetNumGraphs()==2){
			MyPrimitiveDrawer::Scale(DOUBLEGRAPHSIZE);
		}
		else{
			MyPrimitiveDrawer::Scale(SINGLEGRAPHSIZE);
		}
		rep->DrawLegend();
		if(this->DoNeedTick()){
			this->DrawLegendTick();
		}
		MyPrimitiveDrawer::PopMatrix();
	}


	MyMatrixf mat = MyMatrixf::IdentityMatrix();
	MyPrimitiveDrawer::PushProjectionMatrix();
	MyPrimitiveDrawer::PushMatrix();
	MyPrimitiveDrawer::LoadProjectionMatrix(&mat);
	MyPrimitiveDrawer::LoadModelViewMatrix(&mat);

	MyPrimitiveDrawer::WirePolygon();
	MyPrimitiveDrawer::SetLineWidth(3);

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawBitMapTextLarge(MyVec3f(UI_IDX_LEFT,UI_IDX_BOTTOM,0),MyString(mIdx+1)+"/"+MyString(mTotal));
	
	// draw the pause button
	if(mIsPaused){
		MyPrimitiveDrawer::DrawQuadAt(MyVec3f(UI_LEFT,UI_PAUSE_BOX_BOTTOM,0),
			MyVec3f(UI_LEFT,UI_BOX_HEIGHT+UI_PAUSE_BOX_BOTTOM,0),
			MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_BOX_HEIGHT+UI_PAUSE_BOX_BOTTOM,0),
			MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_PAUSE_BOX_BOTTOM,0));
		MyPrimitiveDrawer::DrawBitMapText(MyVec3f(UI_LEFT+UI_BOX_TEXT_LEFT_OFFSET,
			UI_PAUSE_BOX_BOTTOM+UI_BOX_TEXT_BOTTOM_OFFSET,0),"RESUME");
		// restore state
		MyPrimitiveDrawer::PopMatrix();
		MyPrimitiveDrawer::PopMatrix();
		MyPrimitiveDrawer::FillPolygon();
		MyPrimitiveDrawer::SetLineWidth(1);
		return;
	}
	else{
		MyPrimitiveDrawer::DrawQuadAt(MyVec3f(UI_LEFT,UI_PAUSE_BOX_BOTTOM,0),
			MyVec3f(UI_LEFT,UI_BOX_HEIGHT+UI_PAUSE_BOX_BOTTOM,0),
			MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_BOX_HEIGHT+UI_PAUSE_BOX_BOTTOM,0),
			MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_PAUSE_BOX_BOTTOM,0));
		MyPrimitiveDrawer::DrawBitMapText(MyVec3f(UI_LEFT+UI_BOX_TEXT_LEFT_OFFSET,
			UI_PAUSE_BOX_BOTTOM+UI_BOX_TEXT_BOTTOM_OFFSET,0),"PAUSE");
	}


	if (mTask){
		/*
		// draw boxes
		for(int i = 0;i<mTask->GetNumOptions();i++){
			MyString option = mTask->GetOption(i);
			if(i == mHighlightedIndex){
				MyPrimitiveDrawer::Color(MyColor4f(1,1,0,0));
			}
			else{
				MyPrimitiveDrawer::Color(MyColor4f(1,1,1,0));
			}
			MyPrimitiveDrawer::DrawQuadAt(MyVec3f(UI_LEFT,UI_BOTTOM+i*(UI_BOX_HEIGHT+UI_BOX_INTERVAL)+UI_BOX_INTERVAL,0),
				MyVec3f(UI_LEFT,UI_BOTTOM+(i+1)*(UI_BOX_HEIGHT+UI_BOX_INTERVAL),0),
				MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_BOTTOM+(i+1)*(UI_BOX_HEIGHT+UI_BOX_INTERVAL),0),
				MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_BOTTOM+i*(UI_BOX_HEIGHT+UI_BOX_INTERVAL)+UI_BOX_INTERVAL,0));
			MyPrimitiveDrawer::DrawBitMapText(MyVec3f(UI_LEFT+UI_BOX_TEXT_LEFT_OFFSET,
				UI_BOTTOM+i*(UI_BOX_HEIGHT+UI_BOX_INTERVAL)+UI_BOX_INTERVAL+UI_BOX_TEXT_BOTTOM_OFFSET,0),option);
		}
		*/
		
		// draw the cancel button
		if(!mIsPaused){
			MyPrimitiveDrawer::DrawQuadAt(MyVec3f(UI_LEFT,UI_CANCEL_BOX_BOTTOM,0),
				MyVec3f(UI_LEFT,UI_BOX_HEIGHT+UI_CANCEL_BOX_BOTTOM,0),
				MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_BOX_HEIGHT+UI_CANCEL_BOX_BOTTOM,0),
				MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_CANCEL_BOX_BOTTOM,0));
			MyPrimitiveDrawer::DrawBitMapText(MyVec3f(UI_LEFT+UI_BOX_TEXT_LEFT_OFFSET,
				UI_CANCEL_BOX_BOTTOM+UI_BOX_TEXT_BOTTOM_OFFSET,0),"CANCEL");
		}

		// show user's answer
		MyString userAnswer("Unanswered");
		if(mUserSelection >= 0){
			userAnswer = mTask->InterpretSelection(mUserSelection);
		}
		/*
		MyPrimitiveDrawer::DrawQuadAt(MyVec3f(UI_LEFT,UI_BOTTOM+UI_BOX_INTERVAL,0),
				MyVec3f(UI_LEFT,UI_BOTTOM+(UI_BOX_HEIGHT+UI_BOX_INTERVAL),0),
				MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_BOTTOM+(UI_BOX_HEIGHT+UI_BOX_INTERVAL),0),
				MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_BOTTOM+UI_BOX_INTERVAL,0));
		*/
		MyPrimitiveDrawer::DrawBitMapText(MyVec3f(UI_LEFT+UI_BOX_TEXT_LEFT_OFFSET,
			UI_BOTTOM+UI_BOX_INTERVAL+UI_BOX_TEXT_BOTTOM_OFFSET,0),userAnswer);

		MyBoundingBox box = MyPrimitiveDrawer::GetBitMapTextBox(mTask->GetTaskDescription());
		// change resolution from 1200 to 1080
		float offset = -box.GetWidth() / 2;
		//float offset = -box.GetWidth();
		// draw task description
		if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
		else MyPrimitiveDrawer::Color(MyColor4f::white());
		MyPrimitiveDrawer::DrawBitMapTextLarge(MyVec3f(offset,
			UI_DESCRIPTION_BOTTOM, 0), mTask->GetTaskDescription());
		MyPrimitiveDrawer::SetLineWidth(1);

		if (mShowResult && this->GetUserSelection()>=0){
			if (userAnswer == mTask->GetAnswerString()){
				if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::blue());
				else MyPrimitiveDrawer::Color(MyColor4f::yellow());
				MyPrimitiveDrawer::DrawBitMapTextLarge(MyVec3f(UI_RESULT_LEFT,
					UI_RESULT_BOTTOM, 0), "Correct!");
			}
			else{
				MyPrimitiveDrawer::Color(MyColor4f(1,0,0,1));
				MyPrimitiveDrawer::DrawBitMapTextLarge(MyVec3f(UI_RESULT_LEFT,
					UI_RESULT_BOTTOM, 0), "Answer:");
				MyPrimitiveDrawer::DrawBitMapTextLarge(MyVec3f(UI_RESULT_LEFT,
					UI_RESULT_BOTTOM - 0.05, 0), mTask->GetAnswerString());
			}
		}

	}
	
	MyBoundingBox box = MyPrimitiveDrawer::GetBitMapTextBox(mWarningString);
	float offset = -box.GetWidth()/2;
	MyPrimitiveDrawer::DrawBitMapTextLarge(MyVec3f(offset,
		UI_WARNING_BOTTOM, 0), mWarningString);

	float c_dense = 1.f;
	if (!this->IsAnswerSet()){
		c_dense = 0.67;
	}
	if(mFinished){
		MyPrimitiveDrawer::Color(c_dense*MyColor4f(1,1,0,0));
	}
	else{
		if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f((1 - c_dense), (1 - c_dense), (1 - c_dense), 0));
		else MyPrimitiveDrawer::Color(c_dense*MyColor4f::white());
		
	}

	// draw the finish button
	MyPrimitiveDrawer::SetLineWidth(3);
	MyPrimitiveDrawer::DrawQuadAt(MyVec3f(UI_LEFT,UI_FINISH_BOX_BOTTOM,0),
		MyVec3f(UI_LEFT,UI_BOX_HEIGHT+UI_FINISH_BOX_BOTTOM,0),
		MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_BOX_HEIGHT+UI_FINISH_BOX_BOTTOM,0),
		MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_FINISH_BOX_BOTTOM,0));
	MyPrimitiveDrawer::DrawBitMapText(MyVec3f(UI_LEFT+UI_BOX_TEXT_LEFT_OFFSET,
		UI_FINISH_BOX_BOTTOM + UI_BOX_TEXT_BOTTOM_OFFSET, 0), mFinishBoxString);
	MyPrimitiveDrawer::SetLineWidth(1);

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());

	/*
	// draw finish button
	if(mJumpAllowed || mFinished){
		MyPrimitiveDrawer::DrawQuadAt(MyVec3f(UI_LEFT,UI_JUMP_BOX_BOTTOM,0),
			MyVec3f(UI_LEFT,UI_BOX_HEIGHT+UI_JUMP_BOX_BOTTOM,0),
			MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_BOX_HEIGHT+UI_JUMP_BOX_BOTTOM,0),
			MyVec3f(UI_LEFT+UI_BOX_WIDTH,UI_JUMP_BOX_BOTTOM,0));
		MyPrimitiveDrawer::DrawBitMapText(MyVec3f(UI_LEFT+UI_BOX_TEXT_LEFT_OFFSET,
			UI_JUMP_BOX_BOTTOM+UI_BOX_TEXT_BOTTOM_OFFSET,0),"NEXT");
	}
	*/
	MyPrimitiveDrawer::FillPolygon();

	/*
	if (mGraphKnot){
		// draw user current selection list
		if (mGraphKnot->GetSelectionLevel()>0){
			MyString selectionList;
			for (int i = 0; i<mGraphKnot->GetNumSelections(); i++){
				MyString name = dynamic_cast<const MyGenericHierarchyOverlayStr*>(mGraphKnot->GetSelection(i))->GetObject();
				selectionList += (name + " ");
			}
			MyPrimitiveDrawer::DrawBitMapTextLarge(MyVec3f(UI_SELECTION_NAME_LEFT,
				UI_SELECTION_NAME_BOTTOM, 0), selectionList);
		}
	}
	*/

	/*
	int numSegs = 6;
	const MyScale<float, MyColor4f>& colorScale = mGraphKnot->GetRepresentation()->GetColorScale();
	MyColor4f start_color = colorScale.GetRange()[0];
	MyColor4f end_color = colorScale.GetRange()[1];
	float leg_width = (UI_LEGEND_RIGHT-UI_LEGEND_LEFT)/(numSegs-1);
	float leg_height = UI_LEGEND_TOP-UI_LEGEND_BOTTOM;
	float start_value = colorScale.GetDomain()[0];
	float end_value = colorScale.GetDomain()[1];
	if(rep){
		start_value = rep->GetMappingScale().GetDomain()[0];
		end_value = rep->GetMappingScale().GetDomain()[1];
	}
	float inc_value = (end_value-start_value)/(numSegs-1);
	
	MyPrimitiveDrawer::DrawBitMapTextLarge(MyVec3f((UI_LEGEND_RIGHT+UI_LEGEND_LEFT)/2,
		UI_LEGEND_TOP+UI_LEGEND_TEXT_SPACE,0)
		,"Edge Weight");
	for(int i = 0;i<numSegs;i++){
		MyVec3f bl;
		bl[0] = UI_LEGEND_LEFT+i*leg_width;
		bl[1] = UI_LEGEND_BOTTOM-UI_LEGEND_TEXT_SPACE-UI_LEGEND_TEXT_SIZE;
		bl[2] = 0;
		MyString tick(start_value+inc_value*i);
		tick.resize(4);
		MyPrimitiveDrawer::DrawBitMapText(bl,tick);
	}
	
	numSegs = 6;
	leg_width = (UI_LEGEND_RIGHT-UI_LEGEND_LEFT)/(numSegs-1);
	MyPrimitiveDrawer::BeginQuadStrip();
	for(int i = 0;i<numSegs;i++){
		MyColor4f iColor = colorScale[i/(float)numSegs];
		MyPrimitiveDrawer::Color(iColor);
		MyPrimitiveDrawer::Vertex(MyVec2f(UI_LEGEND_LEFT+i*leg_width,UI_LEGEND_TOP));
		MyPrimitiveDrawer::Vertex(MyVec2f(UI_LEGEND_LEFT+i*leg_width,UI_LEGEND_BOTTOM));

	}
	MyPrimitiveDrawer::EndPrimitive();
	
	MyPrimitiveDrawer::WirePolygon();
	MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::BeginQuads();
	MyPrimitiveDrawer::Vertex(MyVec2f(UI_LEGEND_LEFT, UI_LEGEND_BOTTOM));
	MyPrimitiveDrawer::Vertex(MyVec2f(UI_LEGEND_RIGHT, UI_LEGEND_BOTTOM));
	MyPrimitiveDrawer::Vertex(MyVec2f(UI_LEGEND_RIGHT, UI_LEGEND_TOP));
	MyPrimitiveDrawer::Vertex(MyVec2f(UI_LEGEND_LEFT, UI_LEGEND_TOP));
	MyPrimitiveDrawer::EndPrimitive();
	MyPrimitiveDrawer::FillPolygon();
	*/


	MyPrimitiveDrawer::PopProjectionMatrix;
	MyPrimitiveDrawer::PopMatrix();

}

int MyGraphTaskUI:: mouseReleaseEventHandler(MyGenericEvent& eve){
	if(mBeingDrag){
		mBeingDrag = false;
		mTickSet = true;
		MySelectionEvent& seleve = dynamic_cast<MySelectionEvent&>(eve);
		MyVec2f range = this->GetAnswerValueRange();
		float value = (range[1]-range[0])*mTickValue+range[0];
		seleve.SetActionName("UI_LEGEND_DROP");
		seleve.SetActionValue(MyString(value));
	}
	else{
		return 0;
	}
}

int MyGraphTaskUI::mouseMoveEventHandler(MyGenericEvent& eve){
	if(mBeingDrag){
		float mx = eve.GetMouseX();
		float my = eve.GetMouseY();
		int width = eve.GetWindowWidth();
		int height = eve.GetWindowHeight();
		updateTickValue(mx,my,width,height);
		//mTickSet = true;
		eve.SetNeedRedraw();
		MySelectionEvent& seleve = dynamic_cast<MySelectionEvent&>(eve);
		MyVec2f range = this->GetAnswerValueRange();
		float value = (range[1]-range[0])*mTickValue+range[0];
		seleve.SetActionName("UI_LEGEND_MOVE");
		seleve.SetActionValue(MyString(value));
	}
	else{
		return 0;
	}
}

int MyGraphTaskUI::mousePressEventHandler(MyGenericEvent& eve){
	MySelectionEvent& seleve = dynamic_cast<MySelectionEvent&>(eve);
	if(eve.GetEventSource() == MyGenericEvent::MOUSE_KEY
		&& eve.GetEventMouseKey() == MyGenericEvent::MOUSE_KEY_LEFT){
		float mx = eve.GetMouseX();
		float my = eve.GetMouseY();
		int width = eve.GetWindowWidth();
		int height = eve.GetWindowHeight();
		int idx = this->GetHitBoxIndex(mx,my,width, height);
		if(idx < 0) return 0;
		eve.SetNeedRedraw();
		if(idx == UI_PAUSE_BOX_INDEX){
			mIsPaused = !mIsPaused;
			if(mIsPaused){
				seleve.SetActionName("UI_PAUSE");
			}
			else{
				seleve.SetActionName("UI_RESUME");
			}
			return 4;
		}
		else if(this->DoNeedTick()){
			if(idx == UI_LEGEND_BOX_INDEX){
				//mTickSet = true;
				updateTickValue(mx,my,width,height);
				mFinished = false;
				mBeingDrag = true;
				MyVec2f range = this->GetAnswerValueRange();
				float value = (range[1]-range[0])*mTickValue+range[0];
				seleve.SetActionName("UI_LEGEND_PRESS");
				seleve.SetActionValue(MyString(value));
				return 1;
			}
			else if( idx == UI_FINISH_BOX_INDEX){
				seleve.SetActionName("UI_FINISH");
				if(mTickSet){
					mHighlightedIndex == UI_FINISH_BOX_INDEX;
					mFinished = true;
					seleve.SetActionValue("SUCCESS");
					return 2;
				}
				else {
					seleve.SetActionValue("FAIL");
					return 1;
				}
			}
			else if( idx == UI_JUMP_BOX_INDEX && mFinished){
				seleve.SetActionName("UI_NEXT");
				return 3;
			}
			else return 0;
		}
		else{
			if(idx == UI_FINISH_BOX_INDEX){
				if(this->IsAnswerSet()){
					mFinished = true;
					seleve.SetActionName("UI_FINISH");
					seleve.SetActionValue("SUCCESS");
					return 2;
				}
				else{
					seleve.SetActionName("UI_FINISH");
					seleve.SetActionValue("FAILED");
					return 1;
				}
			}
			else if (idx == UI_CANCEL_BOX_INDEX){
				seleve.SetActionName("UI_CANCEL");
				return 5;
			}
			else if(idx == UI_JUMP_BOX_INDEX && (mFinished
				|| mJumpAllowed)){
				seleve.SetActionName("UI_NEXT");
				return 3;
			}
			else{
				mHighlightedIndex = idx;
				mFinished = false;
				mUserSelection = mHighlightedIndex;
				eve.SetNeedRedraw();
				seleve.SetActionName("UI_SELECTION");
				seleve.SetActionValue(mTask->GetOption(mUserSelection));
				return 1;
			}
		}
	}
	return 0;
}

int MyGraphTaskUI::GetHitBoxIndex(int mx, int my, int winWidth, int winHeight) const{
	float x = mx/(float)winWidth*2-1;
	float y = (winHeight-my)/(float)winHeight*2-1;
	int numBox = 0;
	if (mTask){
		numBox = mTask->GetNumOptions();
	}
	MyVec2f p(x,y);
	/*
	for(int i = 0;i<numBox;i++){
		MyVec2f bl(UI_LEFT,UI_BOTTOM+i*(UI_BOX_HEIGHT+UI_BOX_INTERVAL)+UI_BOX_INTERVAL);
		MyVec2f tr(UI_LEFT+UI_BOX_WIDTH,UI_BOTTOM+(i+1)*(UI_BOX_HEIGHT+UI_BOX_INTERVAL));
		if(p>=bl && p<= tr){
			return i;
		}
	}
	*/
	
	MyVec2f nbl(UI_LEFT,UI_FINISH_BOX_BOTTOM);
	MyVec2f ntr(UI_LEFT+UI_BOX_WIDTH,UI_BOX_HEIGHT+UI_FINISH_BOX_BOTTOM);
	if(p>=nbl && p<= ntr){
		return UI_FINISH_BOX_INDEX;
	}

	MyVec2f cbl(UI_LEFT,UI_CANCEL_BOX_BOTTOM);
	MyVec2f ctr(UI_LEFT+UI_BOX_WIDTH,UI_BOX_HEIGHT+UI_CANCEL_BOX_BOTTOM);
	if(p>=cbl && p<= ctr){
		return UI_CANCEL_BOX_INDEX;
	}
	/*
	// draw finish button
	if(mJumpAllowed || mFinished){
		MyVec2f jbl(UI_LEFT,UI_JUMP_BOX_BOTTOM);
		MyVec2f jtr(UI_LEFT+UI_BOX_WIDTH,UI_BOX_HEIGHT+UI_JUMP_BOX_BOTTOM);
		if(p>=jbl && p<= jtr){
			// no option means getting answer from tick
			if(mHighlightedIndex >= 0 || mTask->GetNumOptions() == 0){
				return UI_JUMP_BOX_INDEX;
			}
		}
	}
	*/

	MyVec2f pbl(UI_LEFT,UI_PAUSE_BOX_BOTTOM);
	MyVec2f ptr(UI_LEFT+UI_BOX_WIDTH,UI_BOX_HEIGHT+UI_PAUSE_BOX_BOTTOM);
	if(p>=pbl && p<= ptr){
		// no option means getting answer from tick
		return UI_PAUSE_BOX_INDEX;
	}

	if(mLegendBox.IsIn(MyVec3f(mx,winHeight-my,mLegendBox.GetNear()))){
		return UI_LEGEND_BOX_INDEX;
	}

	// legend handle
	float handle_x = mTickValue*mLegendBox.GetWidth()+mLegendBox.GetLeft();
	MyBoundingBox legendHandle(
		MyVec3f(handle_x-20, mLegendBox.GetBottom()-40,mLegendBox.GetNear()),
		MyVec3f(handle_x+20, mLegendBox.GetBottom(),mLegendBox.GetNear()));
	if(legendHandle.IsIn(MyVec3f(mx,winHeight-my,mLegendBox.GetNear()))){
		return UI_LEGEND_BOX_INDEX;
	}


	return -1;
}

void MyGraphTaskUI::SetUserSelection(int idx){
	mUserSelection = idx;
}

int MyGraphTaskUI::GetUserSelection() const{
	return mUserSelection;
}

float MyGraphTaskUI::GetUserAnswerValue() const{
	float minValue, maxValue;
	const MyGraphEncodingRepresentation *rep = 
		dynamic_cast<const MyGraphEncodingRepresentation *>(mGraphKnot->GetRepresentation());
	if(rep){
		minValue = rep->GetMappingScale().GetDomain()[0];
		maxValue = rep->GetMappingScale().GetDomain()[1];
	}
	return mTickValue*(maxValue-minValue)+minValue;
}

 MyVec2f MyGraphTaskUI::GetAnswerValueRange() const{
	float minValue, maxValue;
	const MyGraphEncodingRepresentation *rep = 
		dynamic_cast<const MyGraphEncodingRepresentation *>(mGraphKnot->GetRepresentation());
	if(rep){
		minValue = rep->GetMappingScale().GetDomain()[0];
		maxValue = rep->GetMappingScale().GetDomain()[1];
	}
	return MyVec2f(minValue, maxValue);
}

void MyGraphTaskUI::SetIndex(int i){
	mIdx = i;
}

void MyGraphTaskUI::DrawLegendTick(){
	/*
	// for vertical legend
	int numTicks = 6;
	MyVec3f bl(0,0,0);
	MyVec3f tr(10,60,0);
	//MyPrimitiveDrawer::DrawLineAt(MyVec3f(tr[0],bl[1],0),tr);
	if(mTickSet){
		MyPrimitiveDrawer::Color(MyColor4f::white());
	}
	else{
		MyPrimitiveDrawer::Color(0.7*MyColor4f::white());
	}
	float y = (tr[1]-bl[1])*mTickValue+bl[1];
	MyPrimitiveDrawer::DrawLineAt(MyVec3f(bl[0],y,0),MyVec3f(tr[0]+3,y,0));
	MyPrimitiveDrawer::BeginTriangleFan();
	MyPrimitiveDrawer::Vertex(MyVec3f(tr[0]+3,y,0));
	MyPrimitiveDrawer::Vertex(MyVec3f(tr[0]+8,y-2,0));
	MyPrimitiveDrawer::Vertex(MyVec3f(tr[0]+8,y+2,0));
	MyPrimitiveDrawer::EndPrimitive();

	if(mShowResult){
		MyVec2f range = this->GetAnswerValueRange();
		float value = mTask->GetAnswerValue();

		MyPrimitiveDrawer::Color(MyColor4f::yellow());
		float y = (tr[1]-bl[1])*(value-range[0])/(range[1]-range[0])+bl[1];
		MyPrimitiveDrawer::DrawLineAt(MyVec3f(bl[0],y,0),MyVec3f(tr[0]+3,y,0));
		MyPrimitiveDrawer::BeginTriangleFan();
		MyPrimitiveDrawer::Vertex(MyVec3f(tr[0]+3,y,0));
		MyPrimitiveDrawer::Vertex(MyVec3f(tr[0]+8,y-2,0));
		MyPrimitiveDrawer::Vertex(MyVec3f(tr[0]+8,y+2,0));
		MyPrimitiveDrawer::EndPrimitive();
	}
	MyVec3f pbl = MyPrimitiveDrawer::GetWindosPosFromWorldPos(bl);
	MyVec3f ptr = MyPrimitiveDrawer::GetWindosPosFromWorldPos(tr);
	mLegendBox = MyBoundingBox(pbl,ptr);
	*/
	
	// for horizontal legend
	int numTicks = 6;
	MyVec3f bl(0,0,0);
	MyVec3f tr(160,MARKERSCALE,0);
	MyBoundingBox box(bl,tr);
	//MyPrimitiveDrawer::DrawLineAt(MyVec3f(tr[0],bl[1],0),tr);
	if(mTickSet){
		//MyPrimitiveDrawer::Color(MyColor4f::white());
		MyPrimitiveDrawer::Color(MyColor4f::black());
	}
	else{
		//MyPrimitiveDrawer::Color(0.7*MyColor4f::white());
		MyPrimitiveDrawer::Color(0.3*MyColor4f::white());
	}
	float x = (tr[0]-bl[0])*mTickValue+bl[0];
	MyPrimitiveDrawer::DrawLineAt(MyVec3f(x,bl[1]-1.5,0),MyVec3f(x,tr[1]+1.5,0));
	MyPrimitiveDrawer::BeginTriangleFan();
	MyPrimitiveDrawer::Vertex(MyVec3f(x,bl[1]-1.5,0));
	MyPrimitiveDrawer::Vertex(MyVec3f(x-2,bl[1]-5,0));
	MyPrimitiveDrawer::Vertex(MyVec3f(x+2,bl[1]-5,0));
	MyPrimitiveDrawer::EndPrimitive();

	if(mShowResult && mTickSet){
		MyVec2f range = this->GetAnswerValueRange();
		float value = mTask->GetAnswerValue();

		MyPrimitiveDrawer::Color(MyColor4f::yellow());
		float xPos = (tr[0]-bl[0])*(value-range[0])/(range[1]-range[0]);
		MyPrimitiveDrawer::DrawLineAt(MyVec3f(xPos,bl[1]-1.5,0),MyVec3f(xPos,tr[1]+1.5,0));
		MyPrimitiveDrawer::BeginTriangleFan();
		MyPrimitiveDrawer::Vertex(MyVec3f(xPos,bl[1]-1.5,0));
		MyPrimitiveDrawer::Vertex(MyVec3f(xPos-2,bl[1]-5,0));
		MyPrimitiveDrawer::Vertex(MyVec3f(xPos+2,bl[1]-5,0));
		MyPrimitiveDrawer::EndPrimitive();
		//MyPrimitiveDrawer::DrawBitMapText(MyVec3f(xPos,bl[1]-7,0),MyString("Correct Value: ")+MyString(value),1);
	}

	if(mBeingDrag){
		MyVec2f range = this->GetAnswerValueRange();
		float value = (range[1]-range[0])*mTickValue+range[0];
		MyPrimitiveDrawer::PushMatrix();
		MyPrimitiveDrawer::Translate(MyVec3f(x,box.GetCenter()[1],0));
		MyGraphEncodingRepresentation* rep = dynamic_cast<MyGraphEncodingRepresentation*>
			(mGraphKnot->GetRepresentation());
		MyPrimitiveDrawer::Scale(MARKERSCALE);
		if(rep->GetVisualElement() == MyVis::SATURATION
			||rep->GetVisualElement() == MyVis::HUE){
			MyPrimitiveDrawer::Translate(MyVec3f(0,1,0));
		}
		rep->DrawMarker(value,std::clock());
		//dynamic_cast<const MyGraphEncodingRepresentation*>
		//	(mGraphKnot->GetRepresentation())->GetMapper().DrawValueMarker(value);
		MyPrimitiveDrawer::PopMatrix();
	}
	MyVec3f pbl = MyPrimitiveDrawer::GetWindowPosFromWorldPos(bl);
	MyVec3f ptr = MyPrimitiveDrawer::GetWindowPosFromWorldPos(tr);
	mLegendBox = MyBoundingBox(pbl,ptr);
}

bool MyGraphTaskUI::DoNeedTick() const{
	// no option means getting answer from tick
	if (!mTask) return false;
	return mTask->GetTaskName() == "Task_Degree";
}

void MyGraphTaskUI::SetJumpOption(bool b){
	mJumpAllowed = b;
}

bool MyGraphTaskUI::IsPaused() const{
	return mIsPaused;
}

void MyGraphTaskUI::SetFinishBoxString(const MyString& str){
	mFinishBoxString = str;
}

void MyGraphTaskUI::updateTickValue(int mx, int my, int winWidth, int winHeight){
	mTickValue = (mx-mLegendBox.GetLeft())/mLegendBox.GetWidth();
	Clamp(mTickValue,0,1);
	// discrete it
	MyVec2f range = this->GetAnswerValueRange();
	int maxDegree = range[1]*(mGraphKnot->GetGraph()->GetNumNodes()-1)+0.5;
	maxDegree *= 2;
	mTickValue = ((int)(mTickValue*maxDegree+0.5))/(float)maxDegree;
}

void MyGraphTaskUI::UpdateUserAnswerFromGraphKnot(){
	if (mGraphKnot){
		if (mGraphKnot->GetSelectionLevel() == 1){
			//mUserSelection = -1;
			if (mGraphKnot->GetNumSelections() > 0){
				/*
				const MyGenericNode* sel = mGraphKnot->GetSelection(0);
				const MyGenericNode* hierarchy = mGraphKnot->GetHierarchy();
				for (int i = 0; i < hierarchy->GetNumChildren(); i++){
					const MyGenericNode* child = hierarchy->GetChild(i);
					if (MyGenericHierarchyOverlayStr::IsObjectEqual(sel, child)){
						mUserSelection = i;
						mHighlightedIndex = i;
						return;
					}
				}
				*/
				const MyGenericNode* sel = mGraphKnot->GetSelection(0);
				MyString name = dynamic_cast<const MyGenericHierarchyOverlayStr*>
					(sel)->GetObject();
				for (int i = 0; i < mTask->GetNumOptions(); i++){
					if (mTask->GetOption(i) == name){
						mUserSelection = i;
						mHighlightedIndex = i;
						return;
					}
				}
			}
		}
	}
}

bool MyGraphTaskUI::IsAnswerSet() const{
	if (!mTask) return true;
	if (this->DoNeedTick()){
		return mTickSet;
	}
	else{
		return (mUserSelection >= 0 && mUserSelection <= 9000);
	}
}


int MyGraphTaskUI::EventHandler(MyGenericEvent& eve){
	if(!mEnabled) return 0;
	else{
		return MyGenericEventListener::EventHandler(eve);
	}
}